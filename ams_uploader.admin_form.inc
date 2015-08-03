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
		foreach ($results as $row) {
			if(!isset($row->name) || empty($row->name)) {
				$row->name = "localhost";
			}


			$form['server_name']['name'] = array(
				'#type' => 'textfield',
				'#title' => t('FQHN'),
				'#required' => TRUE,
				'#default_value' => $row->name,
				);
		}

	}
	else {
		$name = "localhost";
		$form['server_name']['name'] = array(
			'#type' => 'textfield',
			'#title' => t('FQHN'),
			'#required' => TRUE,
			'#default_value' => $name,
			);
	}


    $results2 = db_query('SELECT * FROM ams_uploader_ams_root');

    $count = $results2->rowCount();

    $form['ams_base'] = array(
        '#type' => 'fieldset',
        '#title' => t('Enter the full path to your AMS installation root directory.  This should look like /opt/adobe/ams.'),
    );

    if(isset($results2) && !empty($results2) && $count == 1) {
        // We only have one base installation directory.
        foreach($results2 as $row) {
            $form['ams_base']['ams_base_dir'] = array (
                '#type' => 'textfield',
                '#title' => t('Base Install Path'),
                '#required' => TRUE,
                '#default_value' => $row->path,
            );
        }
    }
    else if ($count == 0) {
        // Initialized without any content.
        $form['ams_base']['ams_base_dir'] = array (
            '#type' => 'textfield',
            '#title' => t('Base Install Path'),
            '#required' => TRUE,
            '#default_value' => '/opt/adobe/ams',
        );
    }
    else {
        throw new Exception("Expected 1 entry in the ams_uploader_ams_root query, got " . $count);
    }

	$results3 = db_query('SELECT * FROM ams_uploader_application_dirs');

	$count = $results3->rowCount();

	$form['app_dirs'] = array(
		'#type' => 'fieldset',
		'#title' => t('Enter the names of the authenticated and non-authenticated application dirs for each protocol: http and rtmp.'),
	);
	$form['app_dirs']['non_auth'] = array(
		'#type' => 'fieldset',
		'#title' => t('Non-Authenticated'),
	);
	$form['app_dirs']['auth'] = array(
		'#type' => 'fieldset',
		'#title' => t('Authenticated'),
	);


	// TODO: allow for more than one application for each auth'd and non-auth'd.
	// TODO: allow for auth'd http application dirs.
	if(isset($results3) && !empty($results3) && $count == 4) {
		// This is the currently acceptable situation.
		foreach ($results3 as $row) {
			if($row->title == 'rtmp_non_auth') {
				// Non Authenticated RTMP
				$form['app_dirs']['non_auth']['rtmp_non_auth'] = array(
					'#type' => 'textfield',
					'#title' => 'RTMP Non-Authenticated',
					'#required' => TRUE,
					'#default_value' => $row->dir,
				);
			}
			else if($row->title == 'http_non_auth') {
				// Non Authenticated HTTP
				$form['app_dirs']['non_auth']['http_non_auth'] = array(
					'#type' => 'textfield',
					'#title' => 'HTTP Non-Authenticated',
					'#required' => TRUE,
					'#default_value' => $row->dir,
				);
			}
			else if($row->title == 'rtmp_auth') {
				// Authenticated RTMP
				$form['app_dirs']['auth']['rtmp_auth'] = array(
					'#type' => 'textfield',
					'#title' => 'RTMP Authenticated',
					'#required' => TRUE,
					'#default_value' => $row->dir,
				);
			}
			// TODO: Implement authenticated http stream handling.
			else if($row->title == 'http_auth') {
				// Authenticated HTTP - not technically supported yet.
				$form['app_dirs']['auth']['http_auth'] = array(
					'#type' => 'textfield',
					'#title' => 'HTTP Authenticated',
					'#required' => FALSE,
					'#default_value' => $row->dir,
				);
			}
			else {
				throw new Exception("Bad value from the ams_uploader_application_dirs table.");
			}
		}
	}
    elseif($count == 0) {
        //Initialized without content
        $form['app_dirs']['non_auth']['rtmp_non_auth'] = array(
            '#type' => 'textfield',
            '#title' => 'RTMP Non-Authenticated',
            '#required' => TRUE,
            '#default_value' => '',
        );
        $form['app_dirs']['non_auth']['http_non_auth'] = array(
            '#type' => 'textfield',
            '#title' => 'HTTP Non-Authenticated',
            '#required' => TRUE,
            '#default_value' => '',
        );
        $form['app_dirs']['auth']['rtmp_auth'] = array(
            '#type' => 'textfield',
            '#title' => 'RTMP Authenticated',
            '#required' => TRUE,
            '#default_value' => '',
        );
        $form['app_dirs']['auth']['http_auth'] = array(
            '#type' => 'textfield',
            '#title' => 'HTTP Authenticated',
            '#required' => FALSE,
            '#default_value' => '',
        );



    }
	else {
		throw new Exception("Expected 4 entries in the ams_uploader_application_dirs query, got " . $count);
	}

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
		'#submit' => array('ams_uploader_admin_form_submit'),
		);

	return $form;

}

function ams_uploader_admin_form_validate($form, &$form_state) {
	$saved_name = $form_state['values']['name'];
	if (strstr($saved_name, 'http')){
		form_set_error("server_name][name", 'Please only include the fully qualified domain name WITHOUT http or anything like it.  e.x. xxx.unm.edu.');
	}
}

function ams_uploader_admin_form_submit($form, &$form_state) {
	$saved_fqdn = $form_state['values']['name'];

	$exists = db_query('SELECT * FROM ams_uploader_admin_vars')->fetchField();

	if(!$exists) {
		db_insert('ams_uploader_admin_vars')
		->fields(array(
			'name' => $saved_fqdn,
			))
		->execute();
	}
	else {
		db_update('ams_uploader_admin_vars')
		->fields(array(
			'name' => $saved_fqdn,
			))
		->execute();
	}


    $exists2 = db_query('SELECT * FROM ams_uploader_ams_root')->fetchField();
    $saved_base_path = $form_state['values']['ams_base_dir'];
    if(!$exists2) {
        db_insert('ams_uploader_ams_root')
            ->fields(array(
                'path' => $saved_base_path,
            ))
            ->execute();
    }
    else {
        db_update('ams_uploader_ams_root')
            ->fields(array(
                'path' => $saved_base_path,
            ))
            ->execute();
    }

    $exists3 = db_query('SELECT * FROM ams_uploader_application_dirs')->fetchField();
    $rtmp_non_auth = $form_state['values']['rtmp_non_auth'];
    $http_non_auth = $form_state['values']['http_non_auth'];
    $rtmp_auth = $form_state['values']['rtmp_auth'];
    $http_auth = $form_state['values']['http_auth'];
    if(!$exists3) {
        db_insert('ams_uploader_application_dirs')
            ->fields(array(
                'title' => 'rtmp_non_auth',
                'dir' => $rtmp_non_auth,
                'protocol' => 'rtmp',
                'authentication' => 0,
            ))
            ->execute();
        db_insert('ams_uploader_application_dirs')
            ->fields(array(
                'title' => 'http_non_auth',
                'dir' => $http_non_auth,
                'protocol' => 'http',
                'authentication' => 0,
            ))
            ->execute();
        db_insert('ams_uploader_application_dirs')
            ->fields(array(
                'title' => 'rtmp_auth',
                'dir' => $rtmp_auth,
                'protocol' => 'rtmp',
                'authentication' => 1,
            ))
            ->execute();
        db_insert('ams_uploader_application_dirs')
            ->fields(array(
                'title' => 'http_auth',
                'dir' => $http_auth,
                'protocol' => 'http',
                'authentication' => 1,
            ))
            ->execute();
    }
    else {
        db_update('ams_uploader_application_dirs')
            ->fields(array(
                'title' => 'rtmp_non_auth',
                'dir' => $rtmp_non_auth,
                'protocol' => 'rtmp',
                'authentication' => 0,
            ))
            ->condition('title', 'rtmp_non_auth', '=')
            ->execute();
        db_update('ams_uploader_application_dirs')
            ->fields(array(
                'title' => 'http_non_auth',
                'dir' => $http_non_auth,
                'protocol' => 'http',
                'authentication' => 0,
            ))
            ->condition('title', 'http_non_auth', '=')
            ->execute();
        db_update('ams_uploader_application_dirs')
            ->fields(array(
                'title' => 'rtmp_auth',
                'dir' => $rtmp_auth,
                'protocol' => 'rtmp',
                'authentication' => 1,
            ))
            ->condition('title', 'rtmp_auth', '=')
            ->execute();
        db_update('ams_uploader_application_dirs')
            ->fields(array(
                'title' => 'http_auth',
                'dir' => $http_auth,
                'protocol' => 'http',
                'authentication' => 1,
            ))
            ->condition('title', 'http_auth', '=')
            ->execute();
    }


	drupal_set_message('Changes saved.');

}