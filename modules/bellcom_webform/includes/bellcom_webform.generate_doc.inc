<?php

/**
 * Renders custom setting for Generate DOC formm.
 *
 * @param $form
 * @param $form_state
 * @param $node
 *
 * @return mixed
 */
function bellcom_webform_generate_doc_settings_form($form, $form_state, $node) {
  $default = bellcom_webform_generate_doc_get_setting($node->nid);

  if (!$default) {
    $default['enabled'] = 0;
  }

  $form['enabled'] = array(
    '#default_value' => $default['enabled'],
    '#type' => 'checkbox',
    '#title' => t('Generate DOC'),
  );

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  $form[] = array(
    '#markup' => '<p>The content of the generated DOC file is the same as for PDF file. It can be altered ' . l('here', 'node/' . $node->nid . '/webform/webform2pdf') . '.</p>',
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
 * Generates and writes the DOC file.
 *
 * @param mixed $webform
 *   Webform object.
 * @param mixed $submission
 *   Submission object.
 *
 * @return string
 *   The URI of the created file.
 */
function bellcom_webform_generate_doc_create_file($webform, $submission) {
  libraries_load('PHPWord');

  // Creating the new document.
  $phpWord = new \PhpOffice\PhpWord\PhpWord();

  // Adding an empty Section to the document.
  $section = $phpWord->addSection();

  $template = _webform2pdf_get_template($webform->nid);
  $text = webform_replace_tokens($template['p_body']['value'], $webform, $submission);
  $text_parts = explode("\n", strip_tags($text));

  foreach($text_parts as $part) {
    if (!empty($part)) {
      $section->addText($part, null, array('space' => array('after' => 300)));
    }
  }

  // Saving the document as OOXML file.
  $file_uri = variable_get('file_temporary_path') . "/webform_submission_" . $webform->nid . "-" . $submission->sid . ".docx";
  $objWriter = \PhpOffice\PhpWord\IOFactory::createWriter($phpWord, 'Word2007');
  $objWriter->save($file_uri);

  return $file_uri;
}

/**
 * Implements submit hook for bellcom_webform_generate_doc_settings_form.
 *
 * @param $form
 * @param $form_state
 */
function bellcom_webform_generate_doc_settings_form_submit($form, &$form_state) {
  $settings = array();

  $settings['nid'] = $form_state['values']['nid'];
  $settings['enabled'] = $form_state['values']['enabled'];

  // First enabled
  if (isset($form_state['values']['new'])) {
    drupal_write_record('bellcom_webform_generate_doc', $settings);
  }
  else {
    drupal_write_record('bellcom_webform_generate_doc', $settings, 'nid');
  }
}

/**
 * Get bellcom_webform setting for the specified nid.
 *
 * @param $nid
 *
 * @return array
 */
function bellcom_webform_generate_doc_get_setting($nid) {
  $default = db_select('bellcom_webform_generate_doc', 'bw')
    ->fields('bw')
    ->condition('nid', $nid, '=')
    ->execute()
    ->fetchAssoc();

  if ($default) {
    return $default;
  }

  return array();
}
