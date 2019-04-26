<?php

/**
 * @file
 * OS2Forms webform address autocomplete linked component.
 */

module_load_include('inc', 'webform', 'components/fieldset');
module_load_include('inc', 'webform', 'components/textfield');
module_load_include('inc', 'webform', 'components/select');
module_load_include('inc', 'os2forms_address_autocomp', 'components/address_autocomp');

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_addrs_autocomp_l() {
  $defaults = _webform_defaults_fieldset();
  $defaults['extra']['address_label_suffix'] = '- ' . t('address');
  $defaults['extra']['matrikula_label_suffix'] = '- ' . t('matrikula');
  return $defaults;
}

/**
 * Implements _webform_edit_component().
 *
 * Used in on component edit page.
 */
function _webform_edit_addrs_autocomp_l($component) {
  $form = array();

  $form['display']['address_label_suffix'] = array(
    '#type' => 'textfield',
    '#title' => t('Address filed postfix text placed to the right of the textfield'),
    '#default_value' => ($component['extra']['address_label_suffix']) ? ($component['extra']['address_label_suffix']) : '',
    '#description' => t('Example: address.'),
    '#size' => 20,
    '#maxlength' => 127,
    '#weight' => 2.2,
    '#parents' => array('extra', 'address_label_suffix'),
  );

  $form['display']['matrikula_label_suffix'] = array(
    '#type' => 'textfield',
    '#title' => t('Matrikula filed postfix text placed to the right of the textfield'),
    '#default_value' => ($component['extra']['matrikula_label_suffix']) ? ($component['extra']['matrikula_label_suffix']) : '',
    '#description' => t('Example: address.'),
    '#size' => 20,
    '#maxlength' => 127,
    '#weight' => 2.3,
    '#parents' => array('extra', 'matrikula_label_suffix'),
  );

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

  $form['#validate'][] = '_webform_edit_addrs_autocomp_l_form_validate';

  return $form;
}

/**
 * Form _webform_edit_address_autocomp validate.
 */
function _webform_edit_addrs_autocomp_l_form_validate($form, &$form_state) {
  $limit_by_municipality = $form_state['values']['extra']['limit_by_municipality'];

  if (!empty($limit_by_municipality)) {
    // Allowing CSV list.
    if (!preg_match('/^(\d+,?)*$/s', $limit_by_municipality)) {
      form_set_error('extra][limit_by_municipality', 'Must be list of integers separated by coma (CSV)');
    }
  }
}

/**
 * Configuration form for the "address_label_suffix" property.
 *
 * Used in form_builder.
 */
function os2forms_address_autocomp_property_address_label_suffix_form(&$form_state, $form_type, $element, $property) {
  $form = array();

  $form['address_label_suffix'] = array(
    '#form_builder' => array('property_group' => 'display'),
    '#type' => 'textfield',
    '#title' => t('Address filed postfix text placed to the right of the textfield'),
    '#default_value' => ($element['#address_label_suffix']) ? $element['#address_label_suffix'] : '',
    '#description' => t('Example: address.'),
    '#size' => 20,
    '#maxlength' => 127,
    '#weight' => 1,
  );

  return $form;
}

/**
 * Configuration form for the "matrikula_label_suffix" property.
 *
 * Used in form_builder.
 */
function os2forms_address_autocomp_property_matrikula_label_suffix_form(&$form_state, $form_type, $element, $property) {
  $form = array();

  $form['matrikula_label_suffix'] = array(
    '#form_builder' => array('property_group' => 'display'),
    '#type' => 'textfield',
    '#title' => t('Matrikula filed postfix text placed to the right of the textfield'),
    '#default_value' => ($element['#matrikula_label_suffix']) ? $element['#matrikula_label_suffix'] : '',
    '#description' => t('Example: matrikula.'),
    '#size' => 20,
    '#maxlength' => 127,
    '#weight' => 2,
  );

  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_addrs_autocomp_l($component, $value = NULL, $filter = TRUE, $submission = NULL) {
  $element = _webform_render_fieldset($component, $value, $filter, $submission);

  // Component address.
  $component_address = $component;
  $component_address['name'] .= empty($component['extra']['address_label_suffix']) ? NULL : ($filter ? ' ' . webform_filter_xss($component['extra']['address_label_suffix']) : ' ' . $component['extra']['address_label_suffix']);
  $component_address['extra'] += _webform_defaults_textfield()['extra'];
  $component_address['extra']['mode'] = 'address';
  $element['address'] = _webform_render_address_autocomp($component_address, $value, $filter, $submission);
  $element['address']['#default_value'] = $value['address'];

  $element['address']['#webform_component'] = $component_address;
  $element['address']['#ajax'] = array(
    'callback' => 'os2forms_address_autocomp_addrs_autocomp_l_update_select_options',
    'event' => 'blur',
    'effect' => 'none',
    'progress' => array(
      'type' => 'none'
    ),
  );
  if (webform_component_feature($component['type'], 'private')) {
    $element['address']['#webform_private'] = $component['extra']['private'];
  }
  $element['address']['#validated'] = TRUE;
  $element['address']['#webform_validated'] = FALSE;
  $element['address']['#weight'] = '-1';

  // Component matrikula.
  $component_matrikula = $component;
  $component_matrikula['name'] .= empty($component['extra']['matrikula_label_suffix']) ? NULL : ($filter ? ' ' . webform_filter_xss($component['extra']['matrikula_label_suffix']) : ' ' . $component['extra']['matrikula_label_suffix']);
  $component_matrikula['extra'] += _webform_defaults_select()['extra'];
  $component_matrikula['extra']['mode'] = 'matrikula';
  $component_matrikula['extra']['multiple'] = FALSE;
  $component_matrikula['extra']['aslist'] = TRUE;

  $element['matrikula'] = _webform_render_select($component_matrikula, $value, $filter, $submission);
  $element['matrikula']['#default_value'] = $value['matrikula'];

  $element['matrikula']['#webform_component'] = $component_matrikula;
  // Showing the element even though options are empty.
  $element['matrikula']['#access'] = TRUE;
  $element['matrikula']['#attributes']['disabled'] = 'disabled';

  if (webform_component_feature($component['type'], 'private')) {
    $element['matrikula']['#webform_private'] = $component['extra']['private'];
  }
  $element['matrikula']['#validated'] = TRUE;
  // Marking element validation on this step so that it accepts new values after
  // ajax repopulate. The value validity is to be checked in a custom validator:
  // os2forms_address_autocomp_addrs_autocomp_l_validate().
  $element['matrikula']['#webform_validated'] = TRUE;

  $element['matrikula']['#prefix'] = '<div class="form-group form-item" id="js-' . $component_matrikula['form_key'] . '-matrikula">';
  $element['matrikula']['#suffix'] = '</div>';


  // If address values is not empty, set the restrictions right away.
  if (!empty($value['address'])) {
    $old_autocomplete_params = $element['matrikula']['#webform_component']['extra'];
    $autocomplete_params = os2forms_address_autocomp_addrs_autocomp_l_generate_autocomplete_params($value['address'], $old_autocomplete_params);

    $element['matrikula']['#options'] = os2forms_address_autocomp_perform_request($autocomplete_params);

    // Updating webform_component['extra'].
    $element['matrikula']['#webform_component']['extra'] = $autocomplete_params + $element['matrikula']['#webform_component']['extra'];

    unset($element['matrikula']['#attributes']['disabled']);
  }

  // Changing matrikula element validation function.
  unset($element['address']['#element_validate']);
  unset($element['matrikula']['#element_validate']);

  $element['#element_validate'] = array('os2forms_address_autocomp_addrs_autocomp_l_validate');

  return $element;
}

/**
 * A Drupal Form API Validation function.
 *
 * Validates the entered values in addrs_autocomp_l field.
 * Also updates the autocomplete path for matrikula field, since it's getting
 * reset if validation fails.
 *
 * @see os2forms_address_autocomp_is_element_valid()
 */
function os2forms_address_autocomp_addrs_autocomp_l_validate($element, &$form_state, $form) {
  // Equals to something like: submitted[address_linked][...].
  $parents = $element['#parents'];

  $value_address =  drupal_array_get_nested_value($form_state['values'], array_merge($parents, array('address')));
  $value_matrikula = drupal_array_get_nested_value($form_state['values'], array_merge($parents, array('matrikula')));

  $address_element = drupal_array_get_nested_value($form_state['complete form'], array_merge($parents, array('address')));
  $matrikula_element = drupal_array_get_nested_value($form_state['complete form'], array_merge($parents, array('matrikula')));

  // Getting matrikula autocomplete params.
  $matrikula_ac_params = $matrikula_element['#webform_component']['extra'];
  $matrikula_ac_params = os2forms_address_autocomp_addrs_autocomp_l_generate_autocomplete_params($value_address, $matrikula_ac_params);

  // Updating matrikula option list.
  $matrikula_element = drupal_array_get_nested_value($form, array_merge($parents, array('matrikula')));

  $matrikula_element['#options'] = os2forms_address_autocomp_perform_request($matrikula_ac_params);
  $matrikula_element = form_process_select($matrikula_element);

  // Updating webform_component['extra'].
  $matrikula_element['#webform_component']['extra'] = $matrikula_ac_params + $matrikula_element['#webform_component']['extra'];

  if (!empty($matrikula_element['#options'])) {
    unset($matrikula_element['#attributes']['disabled']);
  }

  drupal_array_set_nested_value($form_state['complete form'], array_merge($parents, array('matrikula')), $matrikula_element);

  // Getting address autocomplete params.
  $address_ac_path = $address_element['#autocomplete_path'];
  $address_ac_params = os2forms_address_autocomp_extract_params_from_autocomplete_path($address_ac_path);

  if (!empty($value_address) && !os2forms_address_autocomp_is_element_valid($value_address, $address_ac_params)) {
    form_error($element, t('!name: Address value has been changed. Only values from list are allowed.', array('!name' => $element['#title'])));
  }
  if (!empty($value_matrikula) && !os2forms_address_autocomp_is_element_valid($value_matrikula, $matrikula_ac_params)) {
    form_error($element, t('!name: Matrikula value has been changed. Only values from list are allowed.', array('!name' => $element['#title'])));
  }
}

/**
 * Ajax callback to update field select options path.
 */
function os2forms_address_autocomp_addrs_autocomp_l_update_select_options($form, &$form_state) {
  // Equals to something like: submitted[address_linked][...][address].
  $parents = $form_state['triggering_element']['#parents'];
  // Equals to something like: submitted[address_linked][...].
  array_pop($parents);

  // Address element value.
  $address_element_value = drupal_array_get_nested_value($form_state['values'], array_merge($parents, array('address')));

  // Matrikula element.
  $matrikula_element = drupal_array_get_nested_value($form, array_merge($parents, array('matrikula')));

  // Updating the option list.
  if ($address_element_value) {
    $old_autocomplete_params = $matrikula_element['#webform_component']['extra'];
    $autocomplete_params = os2forms_address_autocomp_addrs_autocomp_l_generate_autocomplete_params($address_element_value, $old_autocomplete_params);

    $matrikula_element['#options'] = os2forms_address_autocomp_perform_request($autocomplete_params);

    // Updating webform_component['extra'].
    $matrikula_element['#webform_component']['extra'] = $autocomplete_params + $matrikula_element['#webform_component']['extra'];

    unset($matrikula_element['#attributes']['disabled']);
  }
  else {
    // Empty options and disabling element.
    $matrikula_element['#options'] = array();
    $matrikula_element['#attributes']['disabled'] = 'disabled';
  }

  $matrikula_element = form_process_select($matrikula_element);

  $commands = array();
  $commands[] = ajax_command_replace('#js-' . end($parents) . '-matrikula', drupal_render($matrikula_element));

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Generating autocomplete param array based on values from address field.
 *
 * @param string $address_value
 *   Values from the address field.
 * @param mixed $autocomplete_params
 *   Current autocomplete params.
 *
 * @return array
 *   New autocomplete params.
 */
function os2forms_address_autocomp_addrs_autocomp_l_generate_autocomplete_params($address_value, $autocomplete_params) {
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

  return $autocomplete_params;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_addrs_autocomp_l($component, $value, $format = 'html', $submission = array()) {
  // Component address.
  $component_address = $component;
  $component_address['extra'] += _webform_defaults_textfield()['extra'];
  $component_address['name'] .= empty($component['extra']['address_label_suffix']) ? NULL : ' ' . webform_filter_xss($component['extra']['address_label_suffix']);

  $address_display = _webform_display_textfield($component_address, array($value['address']), $format, $submission);
  if (!isset($address_display['#parents'])) {
    $parents = array('submitted');
    $parents[] = $component['form_key'];
    $address_display['#parents'] = $parents;
  }
  $address_display['#weight'] = -1;

  // Component matrikula.
  $component_matrikula = $component;
  $component_matrikula['extra'] += _webform_defaults_textfield()['extra'];
  $component_matrikula['name'] .= empty($component['extra']['matrikula_label_suffix']) ? NULL : ' ' . webform_filter_xss($component['extra']['matrikula_label_suffix']);

  $matrikula_display = _webform_display_textfield($component_matrikula, array($value['matrikula']), $format, $submission);
  if (!isset($matrikula_display['#parents'])) {
    $parents = array('submitted');
    $parents[] = $component['form_key'];
    $matrikula_display['#parents'] = $parents;
  }

  return array(
    'address' => $address_display,
    'matrikula' => $matrikula_display,
  );
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_addrs_autocomp_l($component, $sids = array(), $single = FALSE, $join = NULL) {
  // This function is not implemented.
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_addrs_autocomp_l($component, $value) {
  return t('Address') . ': ' . _webform_table_textfield($component, array($value['address'])) . '</br>'
    . t('Matrikula') . ': ' . _webform_table_textfield($component, array($value['matrikula']));
}

/**
 * Implements _webform_action_set_component().
 */
function _webform_action_set_addrs_autocomp_l($component, &$element, &$form_state, $value) {
  // This function is not implemented.
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_addrs_autocomp_l($component, $export_options) {
  return _webform_csv_headers_textfield($component, $export_options);
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_addrs_autocomp_l($component, $export_options, $value) {
  return _webform_csv_data_textfield($component, $export_options, array($value['address'])) . ' - ' . _webform_csv_data_textfield($component, $export_options, array($value['matrikula']));
}

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_addrs_autocomp_l() {
  $fields = array();

  $fields['addrs_autocomp_l'] = array(
    'title' => t('Address autocomplete - with matrikula'),
    'weight' => -20,
  );
  $component['name'] = t('New address autocomplete - with matrikula');
  $fields['addrs_autocomp_l']['default'] = _form_builder_webform_default('addrs_autocomp_l', array(), $component);

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_map_component().
 */
function _form_builder_webform_form_builder_map_addrs_autocomp_l() {
  $mapping = array();
  $mapping['form_builder_type'] = 'addrs_autocomp_l';

  $mapping['properties']['address_label_suffix'] = array(
    'storage_parents' => array('extra', 'address_label_suffix'),
  );
  $mapping['properties']['matrikula_label_suffix'] = array(
    'storage_parents' => array('extra', 'matrikula_label_suffix'),
  );
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
