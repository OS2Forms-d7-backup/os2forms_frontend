<?php

/**
 * Renders custom setting for SBSYS form.
 */
function os2forms_sbsys_settings_form($form, $form_state, $node) {
  $components_options = array();
  foreach ($node->webform['components'] as $id => $component) {
    $components_options[$component['form_key']] = $component['name'];
  }
  $components_options['custom'] = t('Custom text') . '...';

  $nemid_field_components_options =  array_merge(array('default_nemid_value' => t('Get value from nemid')), $components_options);
  $default = os2forms_sbsys_get_setting($node->nid);

  if (!$default) {
    $default['enabled'] = 0;
  }

  $form['enabled'] = array(
    '#default_value' => $default['enabled'],
    '#type' => 'checkbox',
    '#title' => t('Send SBSYS XML'),
  );

  $form['mark_synched'] = array(
    '#default_value' => isset($default['mark_synched']) ? $default['mark_synched'] : NULL,
    '#type' => 'checkbox',
    '#title' => t('Mark submission "synched" upon XML sending (schedule for anonymization)'),
    '#states' => array(
      'visible' => array(
        'input[name="enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#description' => t('Confirm that upon XML sending submission is marked as synched, and therefore can be anonymized. <br/><b>NB!</b> If you mark this without making sure the XML can be delivered, you will lose the data.'),
  );

  $form['fields'] = array(
    '#type' => 'fieldset',
    '#title' => t('Field settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#states' => array(
      'visible' => array(
        'input[name="enabled"]' => array('checked' => TRUE),
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

  $form['email'] = array(
    '#type' => 'fieldset',
    '#title' => t('E-mail settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#states' => array(
      'visible' => array(
        'input[name="enabled"]' => array('checked' => TRUE),
      ),
    ),
  );
  $send_mail = array();
  if ( !empty($node->webform['email']) ) {
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
  if ( count($send_mail) ) {
    if (isset($default['no_send_sbsys_email_addr'])) {
      $def = $default['no_send_sbsys_email_addr'];
    }
    if (!isset($def) || !is_array($def) ) {
      $def = array();
    }

    $form['email']['no_send_sbsys_email_addr'] = array(
      '#title' => t("Do not send SBSYS file"),
      '#type' => 'checkboxes',
      '#options' => $send_mail,
      '#default_value' => $def,
      '#description' => t("SBSYS XML file will NOT be attached to the e-mail sent to the following recipients."),
    );
  }

  $form['fields']['MaaSendesTilDFF'] = array(
    '#type' => 'select',
    '#title' => t('@field_name value', array('@field_name' => 'MaaSendesTilDFF')),
    '#options' => array('ja' => 'Ja', 'nej' => 'Nej'),
    '#default_value' => isset($default['MaaSendesTilDFF']) ? $default['MaaSendesTilDFF'] : '',
    '#description' => t('Select a value from form submitted fields or provide a custom static value'),
  );

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  if (!isset($default['nid'])) {
    $form['new'] = array(
      '#type' => 'value',
      '#value' => 1,
    );
  }

  return $form;
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
  $form['fields'][$field_name] = array(
    '#type' => 'select',
    '#title' => t('@field_name value', array('@field_name' => $field_name)),
    '#options' => $components_options,
    '#default_value' => isset($default[$field_name]) ? $default[$field_name] : '',
    '#description' => t('Select a value from form submitted fields or provide a custom static value'),
  );
  if ($field_type == 'nemid_field' && empty($default[$field_name])) {
    $form['fields'][$field_name]['#default_value'] = 'default_nemid_value';
  }
  if ($field_type == 'textfield' || $field_type == 'nemid_field') {
    $form['fields'][$field_name . '_custom'] = array(
      '#type' => 'textfield',
      '#title' => t('@field_name custom text', array('@field_name' => $field_name)),
      '#default_value' => isset($default[$field_name . '_custom']) ? $default[$field_name . '_custom'] : '',
      '#size' => 60,
      '#maxlength' => 128,
      '#states' => array(
        'visible' => array(
          ':input[name="' . $field_name . '"]' => array('value' => 'custom'),
        ),
      ),
      '#description' => t('Provide a custom static value'),
    );
  }
  else {
    $form['fields'][$field_name . '_custom'] = array(
      '#type' => 'textarea',
      '#title' => t('@field_name custom text', array('@field_name' => $field_name)),
      '#default_value' => isset($default[$field_name . '_custom']) ? $default[$field_name . '_custom'] : '',
      '#states' => array(
        'visible' => array(
          ':input[name="' . $field_name . '"]' => array('value' => 'custom'),
        ),
      ),
      '#description' => t('Provide a custom static value'),
    );
  }

  $form['fields'][] = array('#markup' => '<hr/>');
}

/**
 * os2forms_sbsys_settings_form_submit: Implements submit hook
 *
 * @param $form
 * @param $form_state
 */
function os2forms_sbsys_settings_form_submit($form, &$form_state) {
  $save = $form_state['values'];
  unset($save['nid']);
  unset($save['enabled']);
  unset($save['submit']);
  unset($save['reset']);
  unset($save['form_build_id']);
  unset($save['form_token']);
  unset($save['form_id']);
  unset($save['op']);

  // First enabled
  if (isset($form_state['values']['new'])) {
    unset($save['new']);

    if ($form_state['values']['enabled']) {

      $sbsys_settings['nid'] = $form_state['values']['nid'];
      $sbsys_settings['enabled'] = $form_state['values']['enabled'];
      $sbsys_settings['data'] = $save;

      drupal_write_record('os2forms_sbsys_settings', $sbsys_settings);
    }
  }

  if (!isset($form_state['values']['new'])) {
    // update settings
    $sbsys_settings['nid'] = $form_state['values']['nid'];
    $sbsys_settings['enabled'] = $form_state['values']['enabled'];
    $sbsys_settings['data'] = $save;
    drupal_write_record('os2forms_sbsys_settings', $sbsys_settings, 'nid');
  }
}

/**
 * Get os2forms_sbsys setting for the specified nid.
 *
 * @param $nid
 *
 * @return array
 */
function os2forms_sbsys_get_setting($nid) {
  $default = db_select('os2forms_sbsys_settings', 'os')
    ->fields('os')
    ->condition('nid', $nid, '=')
    ->execute()
    ->fetchAssoc();

  if ($default) {
    $data = unserialize($default['data']);
    unset($default['data']);

    return array_merge($default, $data);
  }

  return array();
}

/**
 * Removes sbsys setting from database.
 *
 * @param int $nid
 *   Node id.
 */
function os2forms_sbsys_delete_setting($nid) {
  db_delete('os2forms_sbsys_settings')
    ->condition('nid', $nid, '=')
    ->execute();
}

/**
 * Gets the actual value of the setting by name.
 * If custom text is used, then the custom text will be returned.
 *
 * Returned value is filtered through htmlspecialchars function.
 *
 * @param $name
 * @param $config
 * @param $params
 *
 * @return mixed
 */
function os2forms_sbsys_get_setting_value($name, $config, $params) {
  if ($config[$name] == 'default_nemid_value') {
    return 'default_nemid_value';
  }
  if ($config[$name] != 'custom') {
    return htmlspecialchars(os2forms_sbsys_xml_get_value_by_form_key($config[$name], $params));
  }
  else {
    return htmlspecialchars($config[$name . "_custom"]);
  }
}

/**
 * This function generates the SBSYS XML, fills it with values and returns is as string
 *
 * @param $params
 * @return string
 */
function os2forms_sbsys_generate_xml($params) {
  $nemid_cpr = os2forms_sbsys_xml_get_value_by_type('nemid_cpr', $params);
  $nemid_com_cvr = os2forms_sbsys_xml_get_value_by_type('nemid_com_cvr', $params);
  if (isset($nemid_com_cvr) && !empty($nemid_com_cvr)) {
    $message = cvr_serviceplatformen_query_info($nemid_com_cvr);
    $nemid_name = htmlspecialchars($message['company_name']);
    $nemid_address = htmlspecialchars($message['company_street'] . ' ' . $message['company_house_nr'] . ' ' . $message['company_']);
    $nemid_city = htmlspecialchars($message['company_city']);
    $nemid_zipcode = htmlspecialchars($message['company_zipcode']);
}
  if (isset($nemid_cpr) && !empty($nemid_cpr)) {
    $message = vcv_serviceplatformen_get_address($nemid_cpr);
    $nemid_name = htmlspecialchars($message['name']);
    $nemid_address = htmlspecialchars($message['road'] . ' ' . $message['road_no'] . ' ' . $message['floor'] . ' ' . $message['door']);
    $nemid_city = htmlspecialchars($message['city']);
    $nemid_zipcode = htmlspecialchars($message['zipcode']);
  }

  $config = os2forms_sbsys_get_setting($params['node']->nid);
  $os2formsId = os2forms_sbsys_get_setting_value('os2formsId', $config, $params);
  $kle = os2forms_sbsys_get_setting_value('kle', $config, $params);
  $sagSkabelonId = os2forms_sbsys_get_setting_value('sagSkabelonId', $config, $params);
  $bodyText = os2forms_sbsys_get_setting_value('bodyText', $config, $params);
  $nemid_cpr_mapping_value = os2forms_sbsys_get_setting_value('nemid_cpr', $config, $params);
  $nemid_name_mapping_value = os2forms_sbsys_get_setting_value('nemid_name', $config, $params);
  $nemid_address_mapping_value = os2forms_sbsys_get_setting_value('nemid_address', $config, $params);
  $nemid_zipcode_mapping_value = os2forms_sbsys_get_setting_value('nemid_zipcode', $config, $params);
  $nemid_city_mapping_value = os2forms_sbsys_get_setting_value('nemid_city', $config, $params);
  $maa_sendes_til_dff = isset($config['MaaSendesTilDFF']) ? $config['MaaSendesTilDFF'] : 'ja';
  
  if ($nemid_cpr_mapping_value && $nemid_cpr_mapping_value != 'default_nemid_value') {
    $nemid_cpr = $nemid_cpr_mapping_value;
  }
  if ($nemid_name_mapping_value && $nemid_name_mapping_value != 'default_nemid_value') {
    $nemid_name = $nemid_name_mapping_value;
  }
  if ($nemid_address_mapping_value && $nemid_address_mapping_value != 'default_nemid_value') {
    $nemid_address = $nemid_address_mapping_value;
  }
  if ($nemid_zipcode_mapping_value && $nemid_zipcode_mapping_value != 'default_nemid_value') {
    $nemid_zipcode = $nemid_zipcode_mapping_value;
  }
  if ($nemid_city_mapping_value && $nemid_city_mapping_value != 'default_nemid_value') {
    $nemid_city = $nemid_city_mapping_value;
  }
  $webform = node_load($params['node']->nid);
  $webform_title = htmlspecialchars($webform->title);
  $fields = os2forms_sbsys_xml_get_fields_to_list($params);

  if (isset($fields['antal_rum_max'])) {
    $maxRoom = htmlspecialchars($fields['antal_rum_max']);
  }
  if (isset($fields['antal_rum_min'])) {
    $minRoom = htmlspecialchars($fields['antal_rum_min']);
  }
  if (isset($fields['priority_1'])) {
    $priorities = array();
    for($i=1;  $i<=4; $i++) {
      if ($fields['priority_'.$i])
          $priorities[] = htmlspecialchars($fields['priority_'.$i]);
    }
  }

  $xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<os2formsFormular>
  <OS2FormsId>$os2formsId</OS2FormsId>
  <SBSYSJournalisering>
    <PrimaerPartCprNummer>" . ((!empty($nemid_cpr)) ? $nemid_cpr : '' ) . "</PrimaerPartCprNummer>
    <PrimaerPartCvrNummer>" . ((!empty($nemid_com_cvr)) ? $nemid_com_cvr : '') . "</PrimaerPartCvrNummer>
    <KLe>$kle</KLe>
    <SagSkabelonId>$sagSkabelonId</SagSkabelonId>
  </SBSYSJournalisering>
  <DigitalForsendelse>
    <Slutbruger>
      <CprNummer>" . ((!empty($nemid_cpr)) ? $nemid_cpr : ''  ) . "</CprNummer>
      <CvrNummer>" . ((!empty($nemid_com_cvr)) ? $nemid_com_cvr : '') . "</CvrNummer>
      <Navn>" . ((isset($nemid_name)) ? $nemid_name : '') . "</Navn>
      <Adresse>" . ((isset($nemid_address)) ? $nemid_address : '') . "</Adresse>
      <Postnr>" . ((isset($nemid_zipcode)) ? $nemid_zipcode : '') . "</Postnr>
      <Postdistrikt>" . ((isset($nemid_city)) ? $nemid_city : '') . "</Postdistrikt>
    </Slutbruger>
    <Kvittering>
      <TitelTekst>$webform_title</TitelTekst>
      <BodyTekst>$bodyText</BodyTekst>
    </Kvittering>
    <MaaSendesTilDFF>$maa_sendes_til_dff</MaaSendesTilDFF>";

  if (isset($minRoom) || isset($maxRoom)) {
    $xml .= "<Room>
      <Min>$minRoom</Min>
      <Max>$maxRoom</Max>
    </Room>";
  }

  if (!empty($priorities)) {
    $xml .= "<Omraade>" . implode(',', $priorities) . "</Omraade>";
  }
  $xml .= "</DigitalForsendelse>
  <FormularData>";

  foreach ($fields as $field_name => $field_value) {
    $field_value = htmlspecialchars($field_value);
    $xml .= "<$field_name>$field_value</$field_name>";
  }
  $xml .= "</FormularData>
</os2formsFormular>";
  return $xml;
}

/**
 * Helper function to get element component ID (CID) from the element type
 *
 * @param $type
 * @param $params
 * @return int|string
 */
function os2forms_sbsys_xml_get_cid_by_type($type, $params) {
  $components = $params['node']->webform['components'];

  foreach($components as $cid => $component) {
    if ($component['type'] == $type) {
      return $cid;
    }
  }
}

/**
 * Helper function to get element component ID (CID) from the element form_key
 *
 * @param $type
 * @param $params
 * @return int|string
 */
function os2forms_sbsys_xml_get_cid_by_form_key($form_key, $params) {
  $components = $params['node']->webform['components'];

  foreach($components as $cid => $component) {
    if ($component['form_key'] == $form_key) {
      return $cid;
    }
  }
}


/**
 * Helper function to get element submitted value from the the element type
 *
 * @param $type
 * @param $params
 * @return mixed
 */
function os2forms_sbsys_xml_get_value_by_type($type, $params) {
  $data = $params['submission']->data;

  $cid = os2forms_sbsys_xml_get_cid_by_type($type, $params);
  if ($cid) {
    return reset($data[$cid]);
  }
}

/**
 * Helper function to get element submitted value from the the element form_key
 *
 * @param $form_key
 * @param $params
 * @return mixed
 */
function os2forms_sbsys_xml_get_value_by_form_key($form_key, $params) {
  $data = $params['submission']->data;

  $cid = os2forms_sbsys_xml_get_cid_by_form_key($form_key, $params);
  if ($cid) {
    return reset($data[$cid]);
  }
}

/**
 * Helper function the returns the list of the fields and their values as an array.
 * All field except the field with type markup (used for formatting) are returned
 *
 * @param $params
 * @return array
 */
function os2forms_sbsys_xml_get_fields_to_list($params) {
  $fields_to_list = array();
  $data = $params['submission']->data;
  foreach ($params['node']->webform['components'] as $cid => $element) {
    $field_name = $element['form_key'];
    $field_name = preg_replace('/\W/', '_', $field_name);
    if ($element['type'] == 'markup') {
      $fields_to_list[$field_name] = $element['value'];
    }
    elseif ($data && isset($data[$cid])) {
      $fields_to_list[$field_name] = reset($data[$cid]);
    }
  }
  return $fields_to_list;
}
