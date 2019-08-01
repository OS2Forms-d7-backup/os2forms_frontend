<?php

/**
 * Custom validation for webform2pdf_edit_form form.
 */
function os2forms_webform2pdf_custom_pdf_name_form_validate($form, $form_state) {
  $pdf_name = $form_state['values']['os2forms_webform2pdf_custom_pdf_name'];
  if (!empty($pdf_name) && strcasecmp('.pdf', substr($pdf_name, -4)) !== 0) {
    form_set_error('os2forms_webform2pdf_custom_pdf_name', t('Extension (.pdf) is a mandatory part of the name'));
  }
}

/**
 * Custom form submit that stores the form custom PDF filename.
 */
function os2forms_webform2pdf_custom_pdf_name_form_submit($form, $form_state) {
  $nid = $form['#webform_nid'];
  $data = array(
    'custom_pdf_name' => $form_state['values']['os2forms_webform2pdf_custom_pdf_name'],
  );

  os2forms_frontend_webform_save_setting($nid, 'os2forms_webform2pdf', $data);
}

/**
 * Theme function that returns the custom PDF name for the form. If no name is specified, a default one from theme_webform2pdf_filename is returned.
 *
 * @param $vars
 * @return string
 */
function os2forms_webform2pdf_webform2pdf_filename($vars) {
  $settings = os2forms_frontend_webform_get_setting($vars['node']->nid, 'os2forms_webform2pdf');

  $name = NULL;
  if (isset($settings['custom_pdf_name'])) {
    $name = $settings['custom_pdf_name'];
  }

  if (!empty($name)) {
    $sid = is_object($vars['submission']) ? $vars['submission']->sid : $vars['submission'];

    $name = str_replace(array('@nid', '@sid'), array(
      $vars['node']->nid,
      $sid,
    ), $name);
    return $name;
  } else {
    // call to a default function
    module_load_include('inc', 'webform2pdf', 'includes/webform2pdf.theme');

    return theme_webform2pdf_filename($vars);
  }
}
