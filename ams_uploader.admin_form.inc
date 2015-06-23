<?php

function ams_uploader_admin_form($form, &$form_state) {
	$form = array();

	$form['server_name'] = array(
		'#type' => 'fieldset',
		'#title' => t('Enter the servername of the streaming media server.  This should look like xxx.unm.edu.'),

		);

	$results = db_query('SELECT * FROM ams_uploader_admin_vars');

	$count = $results->rowCount();

	if($count > 1) {
		$form['server_name']['message'] = array(
			'#markup' => '<span style="color:dodgerblue"> For some reason we pulled up more than one name from the database.  This shouldnt happen.  Selecting the first.</span>',
			);
	}

	if(isset($results) && !empty($results) && $count > 0) {
		if(!isset($result->name) || empty($result->name)) {
			$result->name = "libmedia.unm.edu";
		}


		$form['server_name']['name'] = array(
			'#type' => 'textfield',
			'#title' => t('FQHN'),
			'#required' => TRUE,
			'#default_value' => $result->name,
			);
	}
	else {
		$name = "libmedia.unm.edu";
		$form['server_name']['name'] = array(
			'#type' => 'textfield',
			'#title' => t('FQHN'),
			'#required' => TRUE,
			'#default_value' => $name,
			);
	}

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
		'#submit' => array('ams_uploader_admin_form_submit'),
		);

	return $form;

}

function ams_uploader_admin_form_validate($form, &$form_state) {
	$saved_name = $form_state['values']['server_name']['name'];
	if (strstr($saved_name, 'http')){
		form_set_error("server_name][name", 'Please only include the fully qualified domain name WITHOUT http or anything like it.  e.x. xxx.unm.edu.');
	}
}

function ams_uploader_admin_form_submit($form, &$form_state) {
	$saved_name = $form_state['values']['server_name']['name'];

	$exists = db_query('SELECT * FROM ams_uploader_admin_vars')->fetchField();

	if(!exists) {
		db_insert('ams_uploader_admin_vars')
		->fields(array(
			'name' => $saved_name,
			))
		->execute();
	}
	else {
		db_update('ams_uploader_admin_vars')
		->fields(array(
			'name' => $saved_name,
			))
		->execute();
	}

	drupal_set_message('Changes saved.');

}