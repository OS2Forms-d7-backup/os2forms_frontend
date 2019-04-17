<?php

/**
 * @file
 * OS2Forms webform address autocomplete linked component.
 */

module_load_include('inc', 'webform', 'components/fieldset');
module_load_include('inc', 'webform', 'components/textfield');
module_load_include('inc', 'os2forms_address_autocomp', 'components/address_autocomp');

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_addrs_autcomp_l() {
  $defaults = _webform_defaults_fieldset();
  return $defaults;
}

/**
 * Implements _webform_edit_component().
 *
 * Used in on component edit page.
 */
function _webform_edit_addrs_autcomp_l($component) {
  $form = array();

  $form['remove_place_name'] = array(
    '#type' => 'checkbox',
    '#title' => t('Remove place name (address)'),
    '#default_value' => ($component['extra']['remove_place_name']) ? $component['extra']['remove_place_name'] : FALSE,
    '#parents' => array('extra', 'remove_place_name'),
  );

  $form['remove_code'] = array(
    '#type' => 'checkbox',
    '#title' => t('Remove code (matrikula)'),
    '#default_value' => ($component['extra']['remove_code']) ? $component['extra']['remove_code'] : FALSE,
    '#parents' => array('extra', 'remove_code'),
  );

  $form['limit_by_municipality'] = array(
    '#type' => 'textfield',
    '#title' => t('Limit by municipality (-es)'),
    '#default_value' => ($component['extra']['limit_by_municipality']) ? $component['extra']['limit_by_municipality'] : FALSE,
    '#parents' => array('extra', 'limit_by_municipality'),
    '#description' => t('CSV list of municipalities codes, what will limit the address lookup.<br/><b>NB!</b> Jordstykker does not support multiple values'),
  );

  $form['#validate'][] = '_webform_edit_addrs_autcomp_l_form_validate';

  return $form;
}

/**
 * Form _webform_edit_address_autocomp validate.
 */
function _webform_edit_addrs_autcomp_l_form_validate($form, &$form_state) {
  $limit_by_municipality = $form_state['values']['extra']['limit_by_municipality'];

  if (!empty($limit_by_municipality)) {
    // Allowing CSV list.
    if (!preg_match('/^(\d+,?)*$/s', $limit_by_municipality)) {
      form_set_error('extra][limit_by_municipality', 'Must be list of integers separated by coma (CSV)');
    }
  }
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_addrs_autcomp_l($component, $value = NULL, $filter = TRUE, $submission = NULL) {
  $element = _webform_render_fieldset($component, $value, $filter, $submission);

  // Component address.
  $component_address = $component;
  $component_address['name'] .= ' - ' . t('address');
  $component_address['extra']['mode'] = 'address';
  $element['address'] = _webform_render_address_autocomp($component_address, $value, $filter, $submission);
  $element['address']['#default_value'] = $value['address'];
  $element['address']['#webform_component'] = $component_address;
  $element['address']['#ajax'] = array(
    'callback' => 'os2forms_address_automcomplete_addrs_automcomp_l_change_autocomplete_path',
    'event' => 'blur',
    'effect' => 'none',
    'progress' => array(
      'type' => 'none'
    )
  );
  if (webform_component_feature($component['type'], 'private')) {
    $element['address']['#webform_private'] = $component['extra']['private'];
  }
  $element['address']['#validated'] = TRUE;
  $element['address']['#webform_validated'] = FALSE;
  $element['address']['#weight'] = '-1';

  // Component matrikula.
  $component_matrikula = $component;
  $component_matrikula['name'] .= ' - ' . t('matrikula');
  $component_matrikula['extra']['mode'] = 'matrikula';
  $element['matrikula'] = _webform_render_address_autocomp($component_matrikula, $value, $filter, $submission);
  $element['matrikula']['#default_value'] = $value['matrikula'];
  $element['matrikula']['#webform_component'] = $component_matrikula;
  if (webform_component_feature($component['type'], 'private')) {
    $element['matrikula']['#webform_private'] = $component['extra']['private'];
  }
  $element['matrikula']['#validated'] = TRUE;
  $element['matrikula']['#webform_validated'] = FALSE;

  $element['matrikula']['#prefix'] = '<div class="form-group form-item" id="js-' . $component_matrikula['form_key'] . '-matrikula">';
  $element['matrikula']['#suffix'] = '</div>';

  // If address values is not empty, set the restrictions right away.
  if (!empty($value['address'])) {
    $element['matrikula']['#autocomplete_path'] = os2forms_address_automcomplete_addrs_automcomp_l_generate_autocomplete_path($value['address'], $element['matrikula']['#autocomplete_path']);
  }

  return $element;
}

/**
 * Ajax callback to update field autocomplete path.
 */
function os2forms_address_automcomplete_addrs_automcomp_l_change_autocomplete_path($form, &$form_state) {
  // Equals to something like: submitted[address_linked][address].
  $parents = $form_state['triggering_element']['#parents'];

  $address_element_value = $form_state['values'][$parents[0]][$parents[1]]['address'];
  $matrikula_element = $form[$parents[0]][$parents[1]]['matrikula'];

  // Changing the autocomplete path.
  $old_autocomplete_path  = $matrikula_element['#autocomplete_path'];
  $new_autocomplete_path = os2forms_address_automcomplete_addrs_automcomp_l_generate_autocomplete_path($address_element_value, $old_autocomplete_path);
  $matrikula_element['#autocomplete_path'] = $new_autocomplete_path;

  $matrikula_element = form_process_autocomplete($matrikula_element);

  $commands = array();
  $commands[] = ajax_command_insert('#js-' . $parents[1] . '-matrikula', drupal_render($matrikula_element));

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Generating autocomplete path based on valus from address field.
 *
 * @param string $address_value
 *   Values from the address field.
 * @param $autocomplete_path
 *   Current autocomplete path.
 *
 * @return string
 *   New autocomplete path.
 */
function os2forms_address_automcomplete_addrs_automcomp_l_generate_autocomplete_path($address_value, $autocomplete_path) {
  // Getting autocomplete params as string.
  $autocomplete_params_str = str_replace("os2forms_address_autocomp_autocomplete/", '', $autocomplete_path);

  // Getting autocomplete params as array.
  $autocomplete_params = array();
  parse_str($autocomplete_params_str, $autocomplete_params);

  if ($address_value) {
    // Getting address.
    $address = os2forms_address_autocomp_fetch_address($autocomplete_params, $address_value);

    // Getting municipality code from address.
    if ($municipality_code = os2forms_address_autocomp_address_extract_municipality_code($address)) {
      $autocomplete_params['limit_by_municipality'] = $municipality_code;
    }
    // Getting property nr from address.
    if ($property_nr = os2forms_address_autocomp_address_extract_property_nr($address)) {
      $autocomplete_params['property_nr'] = $property_nr;
    }
  }
  else {
    unset($autocomplete_params['limit_by_municipality']);
    unset($autocomplete_params['property_nr']);
  }

  // Generating new autocomplete path
  $new_autocomplete_path = "os2forms_address_autocomp_autocomplete/" . http_build_query($autocomplete_params);

  return $new_autocomplete_path;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_addrs_autcomp_l($component, $value, $format = 'html', $submission = array()) {
  // Component address.
  $component_address = $component;
  $component_address['name'] .= ' - ' . t('address');

  // Component matrikula.
  $component_matrikula = $component;
  $component_matrikula['name'] .= ' - ' . t('matrikula');

  return array(
    'address' => _webform_display_textfield($component_address, array($value['address']), $format, $submission),
    'matrikula' => _webform_display_textfield($component_matrikula, array($value['matrikula']), $format, $submission),
  );
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_addrs_autcomp_l($component, $sids = array(), $single = FALSE, $join = NULL) {
  // This function is not implemented.
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_addrs_autcomp_l($component, $value) {
  return t('Address') . ': ' . _webform_table_textfield($component, array($value['address'])) . '</br>'
    . t('Matrikula') . ': ' . _webform_table_textfield($component, array($value['matrikula']));
}

/**
 * Implements _webform_action_set_component().
 */
function _webform_action_set_addrs_autcomp_l($component, &$element, &$form_state, $value) {
  // This function is not implemented.
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_addrs_autcomp_l($component, $export_options) {
  return _webform_csv_headers_textfield($component, $export_options);
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_addrs_autcomp_l($component, $export_options, $value) {
  return _webform_csv_data_textfield($component, $export_options, array($value['address'])) . ' - ' . _webform_csv_data_textfield($component, $export_options, array($value['matrikula']));
}

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_addrs_autcomp_l() {
  $fields = array();

  $fields['addrs_autcomp_l'] = array(
    'title' => t('Address autocomplete linked'),
    'weight' => -20,
  );
  $component['name'] = t('New address linked');
  $fields['addrs_autcomp_l']['default'] = _form_builder_webform_default('addrs_autcomp_l', array(), $component);

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_map_component().
 */
function _form_builder_webform_form_builder_map_addrs_autcomp_l() {
  $mapping = array();
  $mapping['form_builder_type'] = 'addrs_autcomp_l';

  $mapping['properties']['remove_place_name'] = array(
    'storage_parents' => array('extra', 'remove_place_name'),
  );
  $mapping['properties']['remove_code'] = array(
    'storage_parents' => array('extra', 'remove_code'),
  );
  $mapping['properties']['limit_by_municipality'] = array(
    'storage_parents' => array('extra', 'limit_by_municipality'),
  );

  return $mapping;
}
