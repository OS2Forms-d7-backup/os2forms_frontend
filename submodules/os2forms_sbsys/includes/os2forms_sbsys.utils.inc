<?php

/**
 * Implements hook_form_alter().
 *
 * Adds settings for SBSYS generation to webform.
 *
 * @param $form
 * @param $form_state
 * @param $form_id
 */
function os2forms_sbsys_form_alter(&$form, &$form_state, $form_id) {
  if ($form_id == 'os2forms_settings_form') {
    $nid = $form['#webform_nid'];

    $node = node_load($nid);
    $components_options = array();
    foreach ($node->webform['components'] as $id => $component) {
      $components_options[$component['form_key']] = $component['name'];
    }
    $components_options['custom'] = t('Custom text') . '...';

    $nemid_field_components_options = array_merge(array('default_nemid_value' => t('Get value from nemid')), $components_options);
    $default = os2forms_frontend_webform_get_setting($nid, 'os2forms_sbsys');

    if (!$default) {
      $default['enabled'] = 0;
    }
    $form['os2forms_sbsys_fs'] = array(
      '#type' => 'fieldset',
      '#title' => t('OS2Forms SBSYS XML settings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['os2forms_sbsys_fs']['sbsys_enabled'] = array(
      '#default_value' => $default['enabled'],
      '#type' => 'checkbox',
      '#title' => t('Send SBSYS XML'),
    );
    $form['os2forms_sbsys_fs']['sbsys_mark_synched'] = array(
      '#default_value' => isset($default['mark_synched']) ? $default['mark_synched'] : NULL,
      '#type' => 'checkbox',
      '#title' => t('Mark submission "synched" upon XML sending (schedule for anonymization)'),
      '#states' => array(
        'visible' => array(
          'input[name="sbsys_enabled"]' => array('checked' => TRUE),
        ),
      ),
      '#description' => t('Confirm that upon XML sending submission is marked as synched, and therefore can be anonymized. <br/><b>NB!</b> If you mark this without making sure the XML can be delivered, you will lose the data.'),
    );

    $form['os2forms_sbsys_fs']['fields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Field settings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#states' => array(
        'visible' => array(
          'input[name="sbsys_enabled"]' => array('checked' => TRUE),
        ),
      ),
    );

    //adding os2formsId
    os2forms_sbsys_settings_add_field($form, 'os2formsId', $default, $components_options);
    //adding kle
    os2forms_sbsys_settings_add_field($form, 'kle', $default, $components_options);
    //adding sagSkabelonId
    os2forms_sbsys_settings_add_field($form, 'sagSkabelonId', $default, $components_options);
    //adding nemid_cpr
    os2forms_sbsys_settings_add_field($form, 'nemid_cpr', $default, $nemid_field_components_options, 'nemid_field');
    //adding nemid_name
    os2forms_sbsys_settings_add_field($form, 'nemid_name', $default, $nemid_field_components_options, 'nemid_field');
    //adding nemid_address
    os2forms_sbsys_settings_add_field($form, 'nemid_address', $default, $nemid_field_components_options, 'nemid_field');
    //adding nemid_zipcode
    os2forms_sbsys_settings_add_field($form, 'nemid_zipcode', $default, $nemid_field_components_options, 'nemid_field');
    //adding nemid_city
    os2forms_sbsys_settings_add_field($form, 'nemid_city', $default, $nemid_field_components_options, 'nemid_field');
    //adding bodyText
    os2forms_sbsys_settings_add_field($form, 'bodyText', $default, $components_options, 'textarea');

    $form['os2forms_sbsys_fs']['email'] = array(
      '#type' => 'fieldset',
      '#title' => t('E-mail settings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#states' => array(
        'visible' => array(
          'input[name="sbsys_enabled"]' => array('checked' => TRUE),
        ),
      ),
    );
    $send_mail = array();
    if (!empty($node->webform['email'])) {
      $send_mail['email'] = $node->webform['email'];
    }

    if (is_array($node->webform['emails'])) {
      foreach ($node->webform['emails'] as $eid => $email) {
        $email_addresses = array_filter(explode(',', check_plain($email['email'])));
        foreach ($email_addresses as $key => $email_address) {
          $email_addresses[$key] = webform_format_email_address($email_address, NULL, $node, NULL, FALSE);
        }

        $send_mail[$eid] = implode(', ', $email_addresses);
      }
    }
    if (count($send_mail)) {
      if (isset($default['no_send_sbsys_email_addr'])) {
        $def = $default['no_send_sbsys_email_addr'];
      }
      if (!isset($def) || !is_array($def)) {
        $def = array();
      }

      $form['os2forms_sbsys_fs']['email']['sbsys_no_send_sbsys_email_addr'] = array(
        '#title' => t("Do not send SBSYS file"),
        '#type' => 'checkboxes',
        '#options' => $send_mail,
        '#default_value' => $def,
        '#description' => t("SBSYS XML file will NOT be attached to the e-mail sent to the following recipients."),
      );
    }

    $form['os2forms_sbsys_fs']['fields']['sbsys_MaaSendesTilDFF'] = array(
      '#type' => 'select',
      '#title' => t('@field_name value', array('@field_name' => 'MaaSendesTilDFF')),
      '#options' => array('ja' => 'Ja', 'nej' => 'Nej'),
      '#default_value' => isset($default['MaaSendesTilDFF']) ? $default['MaaSendesTilDFF'] : '',
      '#description' => t('Select a value from form submitted fields or provide a custom static value'),
    );

    $form['#submit'][] = 'os2forms_sbsys_settings_form_submit';
  }
}

/**
 * Generates a field pair - selectbox + custom text field
 *
 * @param $form
 * @param $field_name
 * @param $default
 * @param $components_options
 * @param $field_type
 */
function os2forms_sbsys_settings_add_field(&$form, $field_name, $default, $components_options, $field_type = 'textfield') {
  $form['os2forms_sbsys_fs']['fields']['sbsys_' . $field_name] = array(
    '#type' => 'select',
    '#title' => t('@field_name value', array('@field_name' => $field_name)),
    '#options' => $components_options,
    '#default_value' => isset($default[$field_name]) ? $default[$field_name] : '',
    '#description' => t('Select a value from form submitted fields or provide a custom static value'),
  );
  if ($field_type == 'nemid_field' && empty($default[$field_name])) {
    $form['os2forms_sbsys_fs']['fields']['sbsys_' . $field_name]['#default_value'] = 'default_nemid_value';
  }
  if ($field_type == 'textfield' || $field_type == 'nemid_field') {
    $form['os2forms_sbsys_fs']['fields']['sbsys_' . $field_name . '_custom'] = array(
      '#type' => 'textfield',
      '#title' => t('@field_name custom text', array('@field_name' => $field_name)),
      '#default_value' => isset($default[$field_name . '_custom']) ? $default[$field_name . '_custom'] : '',
      '#size' => 60,
      '#maxlength' => 128,
      '#states' => array(
        'visible' => array(
          ':input[name="' . 'sbsys_' . $field_name . '"]' => array('value' => 'custom'),
        ),
      ),
      '#description' => t('Provide a custom static value'),
    );
  }
  else {
    $form['os2forms_sbsys_fs']['fields']['sbsys_' . $field_name . '_custom'] = array(
      '#type' => 'textarea',
      '#title' => t('@field_name custom text', array('@field_name' => $field_name)),
      '#default_value' => isset($default[$field_name . '_custom']) ? $default[$field_name . '_custom'] : '',
      '#states' => array(
        'visible' => array(
          ':input[name="' . 'sbsys_' . $field_name . '"]' => array('value' => 'custom'),
        ),
      ),
      '#description' => t('Provide a custom static value'),
    );
  }

  $form['os2forms_sbsys_fs']['fields'][] = array('#markup' => '<hr/>');
}

/**
 * os2forms_sbsys_settings_form_submit: Implements submit hook
 *
 * @param $form
 * @param $form_state
 */
function os2forms_sbsys_settings_form_submit($form, &$form_state) {
  if ($form_state['values']['sbsys_enabled']) {
    foreach ($form_state['values'] as $key => $value) {
      if (stripos($key, 'sbsys_') === 0) {
        $data_key = substr($key, 6);
        $data[$data_key] = $value;
      }
    }
  }
  else {
    $data = array(
      'enabled' => $form_state['values']['sbsys_enabled']
    );
  }
  $nid = $form['#webform_nid'];

  os2forms_frontend_webform_save_setting($nid, 'os2forms_sbsys', $data);
}
