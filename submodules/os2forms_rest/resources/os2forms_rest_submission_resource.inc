<?php
/**
 * @file
 * Os2forms_rest_submission_resource.inc.
 */

/**
 * The Valghalla resource definition.
 *
 * @return mixed
 *   Resource definition.
 */
function _os2forms_rest_submission_resource_definition() {
  $resources['os2forms_submission'] = array(
    'operations' => array(
      // Marks submission as synched.
      'delete' => array(
        'file' => array(
          'type' => 'inc',
          'module' => 'os2forms_rest',
          'name' => 'resources/os2forms_rest_submission_resource',
        ),
        'help' => t('Marks unanonymized submission as synched.'),
        'callback' => '_os2forms_rest_submission_resource_delete',
        'args' => array(
          array(
            'name' => 'uuid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'string',
          ),
        ),
        'access arguments' => array('edit all webform submissions'),
      ),
      // Indexes submissions that are added to queue of unanonymized
      // submissions.
      'index' => array(
        'file' => array(
          'type' => 'inc',
          'module' => 'os2forms_rest',
          'name' => 'resources/os2forms_rest_submission_resource',
        ),
        'help' => t('Index all unanonymized submissions, which are not synched.'),
        'callback' => '_os2forms_rest_submission_resource_index',
        'args' => array(
          array(
            'name' => 'page',
            'optional' => TRUE,
            'type' => 'int',
            'description' => 'The zero-based index of the page to get, defaults to 0.',
            'default value' => 0,
            'source' => array('param' => 'page'),
          ),
          array(
            'name' => 'pagesize',
            'optional' => TRUE,
            'type' => 'int',
            'description' => 'Number of records to get per page.',
            'default value' => 100,
            'source' => array('param' => 'pagesize'),
          ),
        ),
        'access arguments' => array('access all webform results'),
      ),
    ),
  );
  // Return the resources.
  return $resources;
}


/**
 * Delete a resource given its uuid.
 *
 * @param string $uuid
 *   UUID of the resource we're deleting.
 *
 * @return int
 *   Number of affected rows.
 */
function _os2forms_rest_submission_resource_delete($uuid) {
  return os2forms_frontend_mark_submission_synched($uuid);
}

/**
 * Return an array of optionally paged webform unanonymized submissions.
 *
 * @param int $page
 *   Page number of results to return (in pages of 100).
 * @param int $pagesize
 *   Integer number of items to be returned.
 *
 * @return array
 *   An array of webform submissions resource objects.
 */
function _os2forms_rest_submission_resource_index($page, $pagesize) {
  return os2forms_frontend_fetch_unanonymized_submissions(0, array('page' => $page, 'pagesize' => $pagesize));
}