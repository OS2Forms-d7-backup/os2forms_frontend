<?php

/**
 * Custom form submit that stores the form custom PDF filename.
 */
function os2forms_nemid_form_submit($form, $form_state) {
  $nid = $form['#webform_nid'];

  $data = array(
    'webform_type' => $form_state['values']['os2form_nemid_webform_type']
  );

  os2forms_frontend_webform_save_setting($nid, 'os2forms_nemid', $data);
}

/**
 * Handles webform fields hiding.
 *
 * Hides the webform fields depending on the webform type and on the type of
 * the nemid, if it is available.
 *
 * @param $form
 *
 * @return array
 *   Returns modified webform;
 */
function os2form_nemid_handle_field_visiblity($form) {
  $nid = $form['#node']->nid;
  $settings = os2forms_frontend_webform_get_setting($nid, 'os2forms_nemid');

  // Getting webform NemID type.
  $webform_type = NULL;
  if (!empty($settings['webform_type'])) {
    $webform_type = $settings['webform_type'];

    // Going through each component to see which of them shall be hidden.
    os2forms_nemid_hide_fields_recursive($form['submitted'], $webform_type);
  }

  // Checking if we have active NemID connection.
  $nemlogin_object = nemlogin_get_plugin_object();
  if (empty($nemlogin_object) || !$nemlogin_object->isInitialized()) {
    return $form;
  }

  // Getting user nemlogin type.
  $nemlogin_type = NULL;
  if ($nemlogin_object->isAuthenticated()) {
    if ($nemlogin_object->fetchValue('cpr')) {
      $nemlogin_type = NEMID_WEBFORM_COMPONENT_TYPE_PERSONAL;
    } elseif ($nemlogin_object->fetchValue('cvr')) {
      $nemlogin_type = NEMID_WEBFORM_COMPONENT_TYPE_COMPANY;
    }
  }

  if ($nemlogin_type) {
    // Hiding irrelevant fields.
    os2forms_nemid_hide_fields_recursive($form['submitted'], $nemlogin_type);

    // If webform and nemlogin types are different, show error message.
    if ($webform_type) {
      if (strcasecmp($nemlogin_type, $webform_type) !== 0) {
        drupal_set_message(t('Your login type does match the login type required by webform. Please log out and sign in differently'), 'warning');
      }
    }
  }

  return $form;
}

/**
 * Checks the if nemid_component_type of a component matches allowed type.
 *
 * If the type does not match, component is not rendered.
 *
 * Works also with the nested structure.
 * Inspired by nemid_populate_fields_recursive
 *
 * @param array $tree
 *   Tree of webform elements.
 * @param string $allowed_type
 *   Allowed type of the component.
 *
 * @see nemid_populate_fields_recursive()
 */
function os2forms_nemid_hide_fields_recursive(array &$tree, $allowed_type) {
  foreach (array_keys($tree) as $item_key) {
    $component_type = NULL;

    if (isset($tree['#tree'])) {
      if (isset($tree[$item_key]['#webform_component']['extra']['nemid_component_type'])) {
        $component_type = $tree[$item_key]['#webform_component']['extra']['nemid_component_type'];
      }

      $item = & $tree[$item_key];
    }
    else {
      if (isset($tree[$item_key]['extra']['nemid_component_type'])) {
        $component_type = $tree[$item_key]['extra']['nemid_component_type'];
      }

      $item = & $tree;
    }

    if ($component_type) {
      if (strcasecmp($component_type, $allowed_type) !== 0) {
        $item['#access'] = FALSE;
      }
    }

    unset($item);
  }

  foreach (array_keys($tree) as $item_key) {
    if (isset($tree[$item_key]['#webform_component'])) {
      if (!empty($tree[$item_key]['#webform_component']['children'])) {
        foreach ($tree[$item_key]['#webform_component']['children'] as $child) {
          os2forms_nemid_hide_fields_recursive($tree[$item_key][$child['form_key']], $allowed_type);
        }
      }
    }
  }
}
 