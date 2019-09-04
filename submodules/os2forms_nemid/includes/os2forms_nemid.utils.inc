<?php

/**
 * Custom form submit that stores the form custom PDF filename.
 */
function os2forms_nemid_form_submit($form, $form_state) {
  $nid = $form['#webform_nid'];

  $data = array(
    'webform_type' => $form_state['values']['os2form_nemid_webform_type'],
    'nemid_link_auto_redirect' => $form_state['values']['os2form_nemid_link_auto_redirect'],
    'nemid_link_enable' => $form_state['values']['os2form_nemid_link_enable']
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
function os2forms_nemid_handle_field_visiblity(&$form) {
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
      $nemlogin_type = OS2FORMS_NEMID_WEBFORM_COMPONENT_TYPE_PERSONAL;
    } elseif ($nemlogin_object->fetchValue('cvr')) {
      $nemlogin_type = OS2FORMS_NEMID_WEBFORM_COMPONENT_TYPE_COMPANY;
    }
  }

  if ($nemlogin_type) {
    // Hiding irrelevant fields.
    os2forms_nemid_hide_fields_recursive($form['submitted'], $nemlogin_type);

    // If webform and nemlogin types are different, show error message.
    if ($webform_type) {
      if (strcasecmp($nemlogin_type, $webform_type) !== 0) {
        drupal_set_message(t('Your login type does match the login type required by webform. Please log out and sign in differently'), 'warning');
        foreach (element_children($form) as $key) {
          $form['actions']['submit']['#disabled'] = TRUE;
        }
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
 
/**
 * Updating fields with the values from nemlogin request.
 *
 * Works also with the nested structure.
 *
 * @param array $tree
 *   Tree of webform elements.
 * @param array $message
 *   Array with messages.
 */
function os2forms_nemid_populate_fields_recursive(array &$tree, array $message) {
  foreach (array_keys($tree) as $item_key) {
    $type = NULL;

    if (isset($tree['#tree'])) {
      $type = $tree[$item_key]['#webform_component']['type'];
      $item = & $tree[$item_key];
    }
    else {
      if (isset($tree[$item_key]['type'])) {
        $type = $tree[$item_key]['type'];
      }
      $item = & $tree;
    }

    switch ($type) {
      case 'nemid_cpr':
        if (isset($message['cpr'])) {
          $item['#value'] = $message['cpr'];
          if ($message['cpr'] != '') {
            $item['#attributes']['readonly'] = 'readonly';
          }
        }
        break;

      case 'nemid_pid':
        if (isset($message['pid'])) {
          $item['#value'] = $message['pid'];
          if ($message['pid'] != '') {
            $item['#attributes']['readonly'] = 'readonly';
          }
        }
        break;

      case 'nemid_address':
        if (isset($message['road'])) {
          $address_text = $message['road'] . ' ' . $message['road_no'] . ' ' . $message['floor'] . ' ' . $message['door'];
          $item['#value'] = $address_text;
          if ($message['road'] != '') {
            $item['#attributes']['readonly'] = 'readonly';
          }
        }
        break;

      case 'nemid_coaddress':
        if (isset($message['coname'])) {
          $item['#value'] = $message['coname'];
          if ($message['coname'] != '') {
            $item['#attributes']['readonly'] = 'readonly';
          }
        }
        break;

      case 'nemid_name':
        if (isset($message['name'])) {
          $item['#value'] = $message['name'];
          if ($message['name'] != '') {
            $item['#attributes']['readonly'] = 'readonly';
          }
        }
        break;

      case 'nemid_city':
        if (isset($message['zipcode'])) {
          $item['#value'] = $message['zipcode'] . ' ' . $message['city'];
          if ($message['zipcode'] != '') {
            $item['#attributes']['readonly'] = 'readonly';
          }
        }
        break;

      case 'nemid_com_cvr':
        if (isset($message['cvr'])) {
          $item['#value'] = $message['cvr'];
          if ($message['cvr'] != '') {
            $item['#attributes']['readonly'] = 'readonly';
          }
        }
        break;

      case 'nemid_com_rid':
        if (isset($message['rid'])) {
          $item['#value'] = $message['rid'];
          if ($message['rid'] != '') {
            $item['#attributes']['readonly'] = 'readonly';
          }
        }
        break;

      case 'nemid_com_name':
        if (isset($message['company_name'])) {
          $item['#value'] = $message['company_name'];
          if ($message['company_name'] != '') {
            $item['#attributes']['readonly'] = 'readonly';
          }
        }
        break;

      case 'nemid_com_addr':
        if (isset($message['company_street'])) {
          $item['#value'] = $message['company_street'] . ' ' . $message['company_house_nr'] . ' ' . $message['company_floor'];
          if ($message['company_street'] != '') {
            $item['#attributes']['readonly'] = 'readonly';
          }
        }
        break;

      case 'nemid_com_city':
        if (isset($message['company_zipcode'])) {
          $item['#value'] = $message['company_zipcode'] . ' ' . $message['company_city'];
          if ($message['company_zipcode'] != '') {
            $item['#attributes']['readonly'] = 'readonly';
          }
        }
        break;
    }

    unset($item);
  }

  foreach (array_keys($tree) as $item_key) {
    if (isset($tree[$item_key]['#webform_component'])) {
      if (!empty($tree[$item_key]['#webform_component']['children'])) {
        foreach ($tree[$item_key]['#webform_component']['children'] as $child) {
          os2forms_nemid_populate_fields_recursive($tree[$item_key][$child['form_key']], $message);
        }
      }
    }
  }
}
