<?php
/**
 * @file
 * ams_uploader_content_type.features.inc
 */

/**
 * Implements hook_node_info().
 */
function ams_uploader_content_type_node_info() {
  $items = array(
    'media' => array(
      'name' => t('Media'),
      'base' => 'node_content',
      'description' => t('Upload video or audio files for the public or UNM personnel only.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  drupal_alter('node_info', $items);
  return $items;
}
