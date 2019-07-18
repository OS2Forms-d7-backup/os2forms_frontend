<?php

/**
 * Custom validation for webform2pdf_edit_form form.
 */
function os2forms_webform2pdf_custom_pdf_name_form_validate($form, $form_state) {
  $pdf_name = $form_state['values']['os2forms_webform2pdf_custom_pdf_name'];
  if (!empty($pdf_name) && strcasecmp('.pdf',substr($pdf_name,-4)) !== 0) {
    form_set_error('os2forms_webform2pdf_custom_pdf_name', t('Extension (.pdf) is a mandatory part of the name'));
  }
}

/**
 * Custom form submit that stores the form custom PDF filename.
 */
function os2forms_webform2pdf_custom_pdf_name_form_submit($form, $form_state) {
  $nid = $form_state['values']['nid'];
  $fields['nid'] = $nid;
  $fields['name'] = $form_state['values']['os2forms_webform2pdf_custom_pdf_name'];

  $query = db_select('os2forms_webform2pdf_settings');
  $query->condition('nid', $nid);
  $query->addExpression('COUNT(*)');
  $rows = $query->execute()->fetchField();

  if ($rows > 0) {
    if (!empty($fields['name'])) {
      db_update('os2forms_webform2pdf_settings')
        ->fields($fields)
        ->condition('nid', $nid)
        ->execute();
    }
    else {
      db_delete('os2forms_webform2pdf_settings')
        ->condition('nid', $nid)
        ->execute();
    }
  }
  else {
    if (!empty($fields['name'])) {
      db_insert('os2forms_webform2pdf_settings')
        ->fields($fields)
        ->execute();
    }
  }
}

/**
 * Theme function that returns the custom PDF name for the form. If no name is specified, a default one from theme_webform2pdf_filename is returned.
 *
 * @param $vars
 * @return string
 */
function os2forms_webform2pdf_webform2pdf_filename($vars) {
  $name = db_select('os2forms_webform2pdf_settings', 'b')
    ->fields('b', array('name'))
    ->condition('nid', $vars['node']->nid)
    ->execute()
    ->fetchField();

  if (!empty($name)) {
    $sid = is_object($vars['submission']) ? $vars['submission']->sid : $vars['submission'];

    $name = str_replace(array('@nid', '@sid'), array($vars['node']->nid, $sid) ,$name);
    return $name;
  } else {
    // call to a default function
    module_load_include('inc', 'webform2pdf', 'includes/webform2pdf.theme');

    return theme_webform2pdf_filename($vars);
  }
}


/**
 * Removes os2forms_webform2pdf setting from database.
 *
 * @param int $nid
 *   Node id.
 */
function os2forms_webform2pdf_delete_setting($nid) {
  db_delete('os2forms_webform2pdf_settings')
    ->condition('nid', $nid, '=')
    ->execute();
}
