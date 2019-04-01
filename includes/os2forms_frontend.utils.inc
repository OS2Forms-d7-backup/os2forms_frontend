<?php

/**
 * @file
 * Os2forms_frontend.utils.inc.
 */

/**
 * Adds submission to the list of unanonymized submissions.
 *
 * @param mixed $submission
 *   Submission object.
 */
function os2forms_frontend_add_unanonymized_submission($submission) {
  $data = array();

  $data['sid'] = $submission->sid;
  $data['uuid'] = $submission->uuid;
  $data['webform_nid'] = $submission->nid;

  $uuid = entity_get_uuid_by_id('node', array($submission->nid));
  if ($uuid && is_array($uuid)) {
    $data['webform_uuid'] = reset($uuid);
  }

  $data['synched'] = 0;

  db_insert('os2forms_frontend_unanonymized_submissions')
    ->fields($data)
    ->execute();
}

/**
 * Deletes submission from the of unanonymized submissions.
 *
 * @param string $uuid
 *   The uuid of the submission.
 *
 * @return int number of affected rows.
 */
function os2forms_frontend_delete_unanonymized_submission($uuid) {
  $row_affected = db_delete('os2forms_frontend_unanonymized_submissions')
    ->condition('uuid', $uuid)
    ->execute();

  return $row_affected;
}

/**
 * Marks unanonymized submission as synched.
 *
 * @param string $uuid
 *   The uuid of the submission.
 *
 * @return int number of affected rows.
 */
function os2forms_frontend_mark_submission_synched($uuid) {
  $data = array();
  $data['synched'] = 1;

  $row_affected = db_update('os2forms_frontend_unanonymized_submissions')
    ->fields($data)
    ->condition('uuid', $uuid)
    ->execute();

  return $row_affected;
}

/**
 * Returns the list of unanonymized submissions.
 *
 * @param int $synched
 *   Which submission to return, 0 indicated unsynched, 1 indicates synched.
 * @param array $pager. If empty all entries will be returned.
 *   Format:
 *   array(
 *     'page' => Number of the page.
 *     'pagesize' => Size of the page.
 *   )
 *
 * * @return array of submissions.
 *
 */
function os2forms_frontend_fetch_unanonymized_submissions($synched = 0, $pager = array()) {
  $query = db_select('os2forms_frontend_unanonymized_submissions', 'o')
    ->fields('o')
    ->condition('synched', $synched);

  if ($pager) {
    $page = $pager['page'];
    $pagesize = $pager['pagesize'];
    $query->range($page * $pagesize, $pagesize);
  }

  $result = $query
    ->execute()
    ->fetchAll();

  return $result;
}

/**
 * Anonymizes a submission of a webform.
 *
 * @param int $webform_nid
 *   Nid of the webform.
 * @param $sid
 *   Sid of the submission.
 */
function os2forms_frontend_anonymize_submission($webform_nid, $sid) {
  $webform = node_load($webform_nid);
  $submission = webform_menu_submission_load($sid, $webform_nid);

  // Iterate through all components and let each do cleanup if necessary.
  foreach ($webform->webform['components'] as $cid => $component) {
    if (isset($submission->data[$cid])) {
      webform_component_invoke($component['type'], 'delete', $component, $submission->data[$cid]);
    }
  }

  // Emptying the fields.
  foreach($submission->data as $key => $element) {
    $submission->data[$key] = array();
  }

  // Updating the submission with empty data.
  webform_submission_update($webform, $submission);

  // Deleting the submission from a queue.
  os2forms_frontend_delete_unanonymized_submission($submission->uuid);
}
