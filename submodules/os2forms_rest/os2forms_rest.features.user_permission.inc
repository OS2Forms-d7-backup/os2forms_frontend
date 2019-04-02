<?php
/**
 * @file
 * os2forms_rest.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function os2forms_rest_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'get any binary files'.
  $permissions['get any binary files'] = array(
    'name' => 'get any binary files',
    'roles' => array(
      'os2forms' => 'os2forms',
    ),
    'module' => 'services',
  );

  return $permissions;
}
