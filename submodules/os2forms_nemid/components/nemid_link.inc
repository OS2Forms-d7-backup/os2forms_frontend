<?php

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_nemid_link() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'extra' => array(
      'nemid_link_login_text' => '',
      'nemid_link_logout_text' => '',
      'nemid_link_auto_redirect' => 0,
      'attributes' => array(),
    ),
  );
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
  $form['extra']['nemid_link_auto_redirect'] = array(
    '#type' => 'checkbox',
    '#title' => t('Redirect to nemlogin link automatically'),
    '#default_value' => $component['extra']['nemid_link_auto_redirect'] ? $component['extra']['nemid_link_auto_redirect'] : 'Logout with Nemlogin',
  );
  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_nemid_link($component, $value = NULL, $filter = TRUE, $submission = NULL) {
  $login_text = $component['extra']['nemid_link_login_text'] ? $component['extra']['nemid_link_login_text'] : 'Login with Nemlogin';
  $logout_text = $component['extra']['nemid_link_logout_text'] ? $component['extra']['nemid_link_logout_text'] : 'Log af Nemlogin';
  $nemlogin_link = nemlogin_link($login_text, $logout_text);

  $element = array(
    '#type' => 'markup',
    '#markup' => $nemlogin_link,
    '#weight' => $component['weight'],
    '#attributes' => $component['extra']['attributes'],
    '#theme_wrappers' => array('webform_element'),
    '#translatable' => array(
      'nemid_link_login_text',
      'nemid_link_logout_text',
    ),
  );

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
  $fields = array();

  $fields['nemid_link'] = array(
    'title' => t('NemID link'),
    'weight' => -19,
  );
  $component['name'] = t('New NemID Link');
  $fields['nemid_link']['default'] = _form_builder_webform_default('nemid_link', array(), $component);
  return $fields;
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
      'nemid_link_auto_redirect' => array(
        'storage_parents' => array('extra', 'nemid_link_auto_redirect'),
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
    'nemid_link_auto_redirect' => array(
      'form' => 'nemid_link_webform_property_nemid_link_auto_redirect_form',
    ),
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

/**
 * Configuration form for the "nemid_link_auto_redirect" property.
 */
function nemid_link_webform_property_nemid_link_auto_redirect_form(&$form_state, $form_type, $element, $property) {
  $component = &$element['#webform_component'];
  $form = array();

  $form['extra']['nemid_link_auto_redirect'] = array(
    '#type' => 'checkbox',
    '#title' => t('Redirect to nemlogin link automatically'),
    '#default_value' => $component['extra']['nemid_link_auto_redirect'] ? $component['extra']['nemid_link_auto_redirect'] : 'Logout with Nemlogin',
  );

  return $form;
}