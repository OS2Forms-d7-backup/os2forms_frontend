<?php

/**
 *Implements hook_form_alter().
 *
 * Adds custom setting for Generate DOC form.
 *
 * @param $form
 * @param $form_state
 * @param $form_id
 */
function os2forms_doc_form_alter(&$form, &$form_state, $form_id) {
  if ($form_id == 'os2forms_settings_form') {
    $nid = $form['#webform_nid'];
    $default = os2forms_frontend_webform_get_setting($nid, 'os2forms_doc');

    if (!$default) {
      $default['enabled'] = 0;
    }
    $form['os2forms_doc_fs'] = array(
        '#type' => 'fieldset',
        '#title' => t('OS2Forms Generate DOC settings'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

    $form['os2forms_doc_fs']['doc_enabled'] = array(
      '#default_value' => $default['enabled'],
      '#type' => 'checkbox',
      '#title' => t('Generate DOC'),
    );

    $form['os2forms_doc_fs'][] = array(
      '#markup' => '<p>The content of the generated DOC file is the same as for PDF file. It can be altered ' . l('here', 'node/' . $nid . '/webform/webform2pdf') . '.</p>',
    );

    $form['#submit'][] = 'os2forms_doc_settings_form_submit';
  }
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
function os2forms_doc_create_file($webform, $submission) {
  libraries_load('PHPWord');

  // Creating the new document.
  $phpWord = new \PhpOffice\PhpWord\PhpWord();

  // Adding an empty Section to the document.
  $section = $phpWord->addSection();

  $template = _webform2pdf_get_template($webform->nid);
  if (empty($template['p_body']['value'])) {
    $template = variable_get('webform2pdf_default', '');
  }

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
 * Implements submit hook for os2forms_doc_settings_form.
 *
 * @param $form
 * @param $form_state
 */
function os2forms_doc_settings_form_submit($form, &$form_state) {
  $nid = $form['#webform_nid'];
  $data = array(
    'enabled' => $form_state['values']['doc_enabled']
  );

  os2forms_frontend_webform_save_setting($nid, 'os2forms_doc', $data);
}
