Drupal.behaviors.portlet = function (context) {

  function getLastSector(id) {
    var elements = id.split('-');
    return elements[elements.length - 1]
  }

  function stopMovingPanel(event, ui) {
		// @todo make for all portlets
		// too order containers
    var portlet = $(ui.item).attr('id');
    var new_container = $(ui.item).parent().attr('id');
    var old_container = $(this).attr('id');
    portlet = getLastSector(portlet);
		if (new_container == 'portlet-tip') {
			old_container = getLastSector(old_container);
			// @todo Add /AJAX
			$.post('container/update', { old_container: old_container, portlet: portlet });
		}
		else {
			new_container = getLastSector(new_container);
			$.post('container/update', { portlet: portlet, new_container: new_container });
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
