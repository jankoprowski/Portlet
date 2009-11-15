<div id="portlet-admin-containers">
<?php foreach ($default as $column => $elements) {
	echo '<fieldset id="portlet-admin-'.check_plain($column).'" class="portlet-admin-js portlet-container-'.$column.'">';
	echo '<legend>'.l(check_plain($column), 'portlet/container/edit/'.check_plain($column)).'</legend>';
  foreach ($elements as $pid) {
		$portlet = $portlets[$pid];
		echo '<span id="portlet-default-'.(int)$portlet->pid.'" class="portlet-admin-thumb portlet-admin-'.(int)$pid.'">'.check_plain($portlet->title).'</span>';
		// Remove portlet which is in container
    unset($portlets[$pid]);
	}
	echo '</fieldset>';
} ?>
</div>

<div class="portlet-admin-js" id="portlet-tip">
<?php foreach ($portlets as $portlet) {
  if (isset($form_state['confirm_delete'])) {
    return array_merge($form, taxonomy_term_confirm_delete($form_state, $edit['tid']));
  }
	if ($portlet->parent != 0) {
		echo '<span id="portlet-default-'.(int)$portlet->pid.'" class="portlet-admin-thumb portlet-admin-'.(int)$portlet->pid.'">'.check_plain($portlet->title).'</span>';
	}
}
?>
</div>

<?php echo drupal_get_form('portlet_container', NULL); ?>
