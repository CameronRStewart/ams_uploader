<?php
/**
 * @file
 * ams_uploader_content_type.features.user_role.inc
 */

/**
 * Implements hook_user_default_roles().
 */
function ams_uploader_content_type_user_default_roles() {
  $roles = array();

  // Exported role: media admin.
  $roles['media admin'] = array(
    'name' => 'media admin',
    'weight' => 3,
  );

  return $roles;
}
