<?php

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_nemid_link() {
  return array_merge_recursive(array(
    'extra' => array(
      'nemid_link_login_text' => '',
      'nemid_link_logout_text' => '',
    ),
  ), os2forms_nemid_webform_defaults_component());
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_nemid_link($component) {
  $form = array();
  $form['extra']['nemid_link_login_text'] = array(
    '#type' => 'textfield',
    '#title' => t('NemID link login text'),
    '#default_value' => $component['extra']['nemid_link_login_text'] ? $component['extra']['nemid_link_login_text'] : 'Login with Nemlogin',
    '#size' => 60,
    '#required' => TRUE,
    '#maxlength' => 255,
    '#weight' => 1.1,
    '#description' => t('Text for login link'),
  );
  $form['extra']['nemid_link_logout_text'] = array(
    '#type' => 'textfield',
    '#title' => t('NemID link logout text'),
    '#default_value' => $component['extra']['nemid_link_logout_text'] ? $component['extra']['nemid_link_logout_text'] : 'Logout with Nemlogin',
    '#size' => 60,
    '#required' => TRUE,
    '#maxlength' => 255,
    '#weight' => 1.2,
    '#description' => t('Text for logout link'),
  );
  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_nemid_link($component, $value = NULL, $filter = TRUE, $submission = NULL) {
  $nid = $component['nid'];
  $settings = os2forms_frontend_webform_get_setting($nid, 'os2forms_nemid');
  $nemid_link_enable = !empty($settings) ? $settings['nemid_link_enable'] : 0;
  if ($nemid_link_enable == 0) {
    $element = array(
      '#type' => 'markup',
      '#markup' => '<p>' . t('In order to add NemID link component, please eneble it first. You can do it ') . l('here', 'node/' . $nid . '/webform/os2forms_settings') . '.</p>',
      '#weight' => $component['weight'],
      '#attributes' => $component['extra']['attributes'],
      '#theme_wrappers' => array('webform_element'),
      '#translatable' => array(
        'nemid_link_login_text',
        'nemid_link_logout_text',
      ),
    );
  }
  else {
    $nemlogin_object = nemlogin_get_plugin_object();
    if (empty($nemlogin_object)) {
      drupal_set_message(t("Nemlogin authorization object is empty."), 'warning');
      return NULL;
    }

    if (!$nemlogin_object->isInitialized()) {
      drupal_set_message(t("Nemlogin authorization object doesn't work properly."), 'warning');
      return NULL;
    }

    if($nemlogin_object->isAuthenticated()) {
      $link_text = $component['extra']['nemid_link_logout_text'] ? $component['extra']['nemid_link_logout_text'] : 'Log af Nemlogin';
      $link_url = os2forms_nemid_get_logout_url();
    } else {
      $link_text = $component['extra']['nemid_link_login_text'] ? $component['extra']['nemid_link_login_text'] : 'Login with Nemlogin';
      $link_url = os2forms_nemid_get_login_url();
    }

    $element = array(
      '#type' => 'link',
      '#title' => $link_text,
      '#href' => $link_url,
      '#title_display' => 'none',
      '#weight' => $component['weight'],
      '#attributes' => $component['extra']['attributes'],
      '#theme_wrappers' => array('webform_element'),
      '#translatable' => array(
        'nemid_link_login_text',
        'nemid_link_logout_text',
      ),
    );
  }  

  return $element;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_nemid_link($component, $value, $format = 'html', $submission = array()) {
  return array(
    '#weight' => $component['weight'],
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#translatable' => array('nemid_link_login_text', 'nemid_link_logout_text'),
  );
}

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_nemid_link() {
  return os2forms_nemid_form_builder_webform_form_builder_types_nemid_com_name('nemid_link', t('Link'));
}

/**
 * Implements _form_builder_webform_form_builder_map_component().
 */
function _form_builder_webform_form_builder_map_nemid_link() {
  return array(
    'form_builder_type' => 'nemid_link',
    'properties' => array(
      'nemid_link_login_text' => array(
        'storage_parents' => array('extra', 'nemid_link_login_text'),
      ),
      'nemid_link_logout_text' => array(
        'storage_parents' => array('extra', 'nemid_link_logout_text'),
      ),
    ),
  );
 }

 /**
 * Implements _form_builder_webform_form_builder_properties_component().
 */
function _form_builder_webform_form_builder_properties_nemid_link() {
  return array(
    'nemid_link_login_text' => array(
      'form' => 'nemid_link_webform_property_nemid_link_login_text_form',
    ),
    'nemid_link_logout_text' => array(
      'form' => 'nemid_link_webform_property_nemid_link_logout_text_form',
    ),
   'default_value' => array('form' => 'nemid_link_webform_property_nemid_link_auto_redirect_form',)
  );
}


/**
 * Configuration form for the "nemid_link_login_text" property.
 */
function nemid_link_webform_property_nemid_link_login_text_form(&$form_state, $form_type, $element, $property) {
  $component = &$element['#webform_component'];
  $form = array();

  $form['nemid_link_login_text'] = array(
    '#form_builder' => array('property_group' => 'properties'),
    '#type' => 'textfield',
    '#title' => t('NemID link login text'),
    '#default_value' => $component['extra']['nemid_link_login_text'] ? $component['extra']['nemid_link_login_text'] : 'Login with Nemlogin',
    '#size' => 60,
    '#required' => TRUE,
    '#maxlength' => 255,
    '#weight' => 1.1,
  );

  return $form;
}

/**
 * Configuration form for the "nemid_link_logout_text" property.
 */
function nemid_link_webform_property_nemid_link_logout_text_form(&$form_state, $form_type, $element, $property) {
  $component = &$element['#webform_component'];
  $form = array();

  $form['nemid_link_logout_text'] = array(
    '#form_builder' => array('property_group' => 'properties'),
    '#type' => 'textfield',
    '#title' => t('NemID link logout text'),
    '#default_value' => $component['extra']['nemid_link_logout_text'] ? $component['extra']['nemid_link_logout_text'] : 'Login with Nemlogin',
    '#size' => 60,
    '#required' => TRUE,
    '#maxlength' => 255,
    '#weight' => 1.1,
  );

  return $form;
}
