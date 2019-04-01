<?php
/**
 * The webform resource definition.
 *
 * @return array
 */

function _os2forms_rest_webform_resource_definition() {
  // Define the resources.
  $resources = array();

  // Add the resource type.
  $resources['os2forms_webform'] = array(
    'operations' => array(
      'retrieve' => array(
        'file' => array(
          'type' => 'inc',
          'module' => 'os2forms_rest',
          'name' => 'resources/os2forms_rest_webform_resource'
        ),
        'help' => t('Return webforms as exported node in serialize format.'),
        'callback' => '_os2forms_rest_webform_resource_retrieve',
        'args' => array(
          array(
            'name' => 'uuid',
            'optional' => FALSE,
            'source' => array('path' => 0),
            'type' => 'string',
            'description' => 'The uuid of the node to get',
          ),
        ),
        'access arguments' => array('access content'),
      ),
    ),
  );

  // Return the resources.
  return $resources;
}

/**
 * Returns a single node resource as exported node with serialize format.
 *
 * @param $entity
 *   UUID or the whole object of the media we want to return.
 * @return
 *   Serialized node object or empty array.
 *
 * @see node_export()
 */
function _os2forms_rest_webform_resource_retrieve($uuid) {
  $resource = array();

  $entities = entity_uuid_load('node', array($uuid), array('type' => 'webform'), TRUE);
  if (isset($entities) && is_array($entities)) {
    $entity = reset($entities);
  }

  if ($entity) {
    // Exporting with serialize formatter.
    $export = node_export(intval($entity->nid), 'serialize');

    if ($export['success'] && !empty($export['output'])) {
      $resource = $export['output'];
    }
    else {
      $resource = array();
    }
  }

  // Return the resource.
  return $resource;
}
