<?php

/**
 * @file
 * Os2forms_frontend.pages.inc.
 */

/**
 * Renders custom setting OS2Forms.
 *
 * OS2Forms frontend does not have any settings of its own, but that form will
 * be used from OS2Forms submodules to hook into and provide extra settings.
 */
function os2forms_settings_form($form, $form_state, $node) {
  $form = array();

  // Exposing webform nid for submodules to use.
  $form['#webform_nid'] = $node->nid;

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 99,
  );

  return $form;
}
