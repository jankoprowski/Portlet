<?php
/**
 * Method return array of portlets types
 * to add in admin panel.
 */
function portlet_root_options() {
    return array('root' => 'root');
}

function portlet_edit_form($form_state, $portlet) {
	// @todo Unit the existing "new" and "edit" forms to one :)
	$args = array();
  $types = variable_get('portlet_types', array());
  foreach ($types as $module => $type) {
    $args = array_merge($args, call_user_func('portlet_' . $module . '_options', ''));
	}

	$parents = array('None');
  $result = db_query('SELECT pid, title FROM {portlet_config} WHERE type="root" ORDER BY title;');
  while($_portlet = db_fetch_array($result)) {
    $parents[$_portlet['pid']] = $_portlet['title'];
  }
  
  $form = array();
  $form['portlet'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#title' => t('Edit portlet')
	);
	$form['portlet']['pid'] = array(
		'#type' => 'hidden',
		'#value' => $portlet->pid
	);
  $form['portlet']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $portlet->title,
    '#description' => t('Enter title of portlet')
	);
	$attributes = (0 != (int)$portlet->countable) ? array('checked' => 'checked') : array();
  $form['portlet']['countable'] = array(
    '#type' => 'checkbox',
		'#title' => 'Countable',
		'#attributes' => $attributes
	);
  $form['portlet']['type'] = array(
		'#type' => 'select',
		'#title'=> 'Type',
		'#default_value' => $portlet->type.':'.$portlet->arg,
		'#options' => $args,
  );
	$form['portlet']['parent'] = array(
		'#type' => 'select',
		'#title' => t('Parent'),
		'#default_value' => array($portlet->parent),
		'#options' => $parents
	);
  $form['portlet']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
	);
  if (! empty($portlet)) {
    $form['portlet']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete')
    );
  }
  return $form;
}

function portlet_edit_form_validate($form, &$form_state) {
	if ($form_state['values']['op'] == t('Save')) {
		if (empty($form_state['values']['title'])) {
			form_set_error('title', t('New portlet must have title'));
		}
	}
}

function portlet_edit_form_submit($form, &$form_state) {
	if ($form_state['values']['op'] == t('Save')) {
		list($type, $arg) = explode(':', $form_state['values']['type']);
		db_query('UPDATE {portlet_config} SET title = "%s", countable = %d, type = "%s", arg = "%s", parent = "%s" WHERE pid = %d;', $form_state['values']['title'], $form_state['values']['countable'], $type, $arg, $form_state['values']['parent'], $form_state['values']['pid']);
	}
  if ($form_state['values']['op'] == t('Delete')) {
		db_query('DELETE FROM {portlet_config} WHERE pid = %d', $form_state['values']['pid']);
		db_query('DELETE FROM {portlet_guests} WHERE pid = %d', $form_state['values']['pid']);
		db_query('DELETE FROM {portlet_users} WHERE pid = %d', $form_state['values']['pid']);
  }
  $form_state['redirect'] = 'admin/build/portlet';
}

/**
 * Form to use sorting
 */
function portlet_admin_form() {
  $form = array(
    '#theme' => 'portlet_admin_form',
    '#tree' => TRUE
  );

  $portlets = portlet_get_tree();
    // echo '<pre>';
    // print_r($portlets);
    // echo '</pre>';  
    // @trash
  $types = variable_get('portlet_types', array());
	// @todo - extract to function
  $args = array();
	$roots = array();

  foreach ($types as $module => $type) {
    $args = array_merge($args, call_user_func('portlet_' . $module . '_options', ''));
	}
	// @todo - extract to function
  $result = db_query('SELECT pid, title FROM {portlet_config} WHERE type="root" ORDER BY title;');
  while($portlet = db_fetch_array($result)) {
    $roots[$portlet['pid']] = $portlet['title'];
  }

  foreach($portlets as $portlet) {
    $key = 'pid:' . $portlet->pid ;
    $form[$key]['#portlet'] = (array)$portlet;

    $form[$key]['pid'] = array(
        '#type' => 'hidden',
        '#value' => $portlet->pid
    );
    $form[$key]['title'] = array(
      '#value' => $portlet->title
    );
    $form[$key]['weight'] = array(
      '#type' => 'weight',
      '#default_value' => $portlet->weight
    );
    $form[$key]['parent'] = array(
      '#type' => 'hidden',
      // Yes, default_value on a hidden. It needs to be changeable by the javascript.
      '#default_value' => $portlet->parent
    );
	}

	// @todo Replace by calling function
	// and remove unecessery thigs from the begging
  $form['new']['title'] = array(
    '#type' => 'textfield'
  );
  $form['new']['countable'] = array(
    '#type' => 'checkbox'
  );
  $form['new']['type'] = array(
    '#type' => 'select',
    '#options' => $args
  );
  $form['new']['parent'] = array(
    '#type' => 'select',
    '#options' => $roots
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
	);
	$form['add'] = array(
		'#type' => 'submit',
		'#value' => t('Add')
	);
  return $form;
}

/**
 * Theme to portlet
 */
function theme_portlet_admin_form($form) {
  $header = array(t('Title'), t('Weight'), t('Operations'));
  $rows = array();
  drupal_add_tabledrag('portlet-categorization', 'match', 'parent', 'portlet-parent', 'portlet-parent', 'portlet-id', TRUE, 2);
  drupal_add_tabledrag('portlet-categorization', 'order', 'sibling', 'portlet-weight');
  
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['#portlet'])) {
    
      $classes = array('draggable');
      if ($form[$key]['#portlet']['type'] == 'root') {
        $classes[] = 'tabledrag-root';
      } else {
        $classes[] = 'tabledrag-leaf';
      }

      $form[$key]['weight']['#attributes']['class'] = 'portlet-weight';
      $form[$key]['parent']['#attributes']['class'] = 'portlet-parent';
      $form[$key]['pid']['#attributes']['class'] = 'portlet-id';

      $row = array();
      $row[] = theme('indentation', (int)($form[$key]['#portlet']['parent'] != 0)) . drupal_render($form[$key]['title']); 
      $row[] = drupal_render($form[$key]['parent']) . drupal_render($form[$key]['pid']) . drupal_render($form[$key]['weight']);
      $row[] = l(t('edit'), 'admin/build/portlet/edit/'.(int)$form[$key]['#portlet']['pid']);
      
      $rows[] = array('data' => $row, 'class' => implode(' ', $classes));
    }
  }
  $output  = theme('table', $header, $rows, array('id' => 'portlet-categorization'));

  $header = array(t('Title'), t('Countable'), t('Portlet'), t('Parent'));
  $rows = array();

  $row = array();
  $row[] = drupal_render($form['new']['title']);
  $row[] = drupal_render($form['new']['countable']);
  $row[] = drupal_render($form['new']['type']);
  $row[] = drupal_render($form['new']['parent']);
  $rows[] = array('data' => $row);
  $output .= theme('table', $header, $rows);

	$output .= drupal_render($form);
  return $output;
}

function portlet_admin_form_validate($form, &$form_state) {
	if ($form_state['values']['op'] == t('Add')) {
		if (empty($form_state['values']['new']['title'])) {
			form_set_error('new][title', t('New portlet must have title'));
		}
	}
}

function portlet_admin_form_submit($form, &$form_state) {    
	if ($form_state['values']['op'] == t('Add')) {
		$new =& $form_state['values']['new'];
		if ($new['type']) {
			if ($new['type'] == 'root') {
				$new['parent'] = 0;
				$new['arg'] = '';
			} else {
				list($new['type'], $new['arg']) = explode(':', $new['type']);
			}
            //die(var_dump($new));			
			db_query('INSERT INTO {portlet_config} (title, countable, arg, type, parent) VALUES ("%s", "%s", "%s", "%s", %d)', $new['title'], $new['countable'], $new['arg'], $new['type'], $new['parent']);
		}
	}

	 // Save new order of portlets
	 // @todo Save only modified!
	 if ($form_state['values']['op'] == t('Save')) {
		 foreach($form_state['values'] as $key => $portlet) {
			db_query('UPDATE {portlet_config} SET parent=%d, weight=%d WHERE pid=%d;', $portlet['parent'], $portlet['weight'], $portlet['pid']);
		 }
		 drupal_set_message(t('Your panels configuration was saved.'));
	 }
}

function portlet_default() {
  drupal_add_css(drupal_get_path('module', 'portlet') . '/portlet.admin.css');
  jquery_ui_add('ui.sortable');
  drupal_add_js(drupal_get_path('module', 'portlet') .'/portlet.admin.js');
  $portlets = portlet_get_all();
	$default = variable_get('portlet_default', array());
  return theme('portlet_default', $default, $portlets);
}

function portlet_update_in_default() {
  $types = variable_get('portlet_default', array());
	// Remove portlet from old location
	if (isset($_POST['old_container'])) {
		$portlet = $_POST['portlet'];
		$old_container = $_POST['old_container'];
		$count = count($types);
		// @todo make a function "remove_element"
		for ($i = 0; $i < $count; $i++) {
			if ($types[$old_container][$i] == $portlet) {
				unset($types[$old_container][$i]);
				break;
			}
		}
		//unset($types[$old_container][$portlet]);
	}
	// If new location add to location
  if (isset($_POST['new_container'])) {
		$new_container = $_POST['new_container'];
    $portlets = explode(',', $_POST['new_order']);
		unset($types[$new_container]);
    foreach ($portlets as $portlet) {
      $data = explode('-', $portlet);
			$pid = $data[2];
      $types[$new_container][] = $pid;
    }
	}
	variable_set('portlet_default', $types);
	exit;
}

function portlet_container($form_state, $container) {
  $form = array();
  $form['container'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#title' => t('Add container')
  );
  $form['container']['old_title'] = array(
    '#type' => 'hidden',
    '#value' => $container->title
  );
  $form['container']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $container->title,
    '#description' => t('Enter title of new container')
  );
  $form['container']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
	);
	if (empty($container)) {
		$form['container']['reset'] = array(
			'#type' => 'submit',
			'#value' => t('Reset')
		);
	}
  if (! empty($container)) {
    $form['container']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete')
    );
  }
  return $form;
}

function portlet_container_validate($form, &$form_state) {
  if ($form_state['values']['op'] == t('Save')) {
		if (empty($form_state['values']['title'])) {
			form_set_error('title', t('Container must have title'));
		}
	}
}

function portlet_container_submit($form, &$form_state) {
  $types = variable_get('portlet_default', array());
  $old_title = $form_state['values']['old_title'];
  $title = $form_state['values']['title'];

  if ($form_state['values']['op'] == t('Delete')) {
    unset($types[$old_title]);
  }

  if ($form_state['values']['op'] == t('Save')) {
    if (isset($types[$old_title]) AND $old_title != $title) {
      $types[$title] = $types[$old_title];
      unset($types[$old_title]);
    } else {
      $types[$title] = array();
    }
	}
	if ($form_state['values']['op'] == t('Reset')) {
		$types = array();
	}
  $form_state['redirect'] = 'admin/build/portlet/default';

  variable_set('portlet_default', $types);

  return;
}
?>
