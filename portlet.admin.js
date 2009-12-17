Drupal.behaviors.portlet = function (context) {

  function getLastSector(id) {
    var elements = id.split('-');
    return elements[elements.length - 1]
  }

  function stopMovingPanel(event, ui) {
		// @todo make for all portlets
		// too order containers
		// @todo save also order of portlets in container
		// @todo change container's order
    var portlet = $(ui.item).attr('id');
    var new_container = $(ui.item).parent().attr('id');
    var old_container = $(this).attr('id');
		old_container = getLastSector(old_container);
    portlet = getLastSector(portlet);
		if (new_container == 'portlet-tip') {
			// @todo Add /AJAX
			$.post('container/update', { old_container: old_container, portlet: portlet });
		} else {
			// @todo Switch array sending to json
			new_container = getLastSector(new_container);
			var new_order = String($(ui.item).parent().sortable('toArray'));
			// Pass also old container to remove from old place
			$.post('container/update', { portlet: portlet, new_container: new_container, new_order: new_order, old_container: old_container });
		}
  }

  $('.portlet-admin-js > span').css('cursor', 'move');

  var columns = ['.portlet-admin-js'];
  var $columns = $(columns.toString());

  $columns.sortable({
		items: '> span',
		cursor: 'move',
		revert: true,
    connectWith: $columns,
    stop: stopMovingPanel
  });
}
