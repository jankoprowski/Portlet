Drupal.behaviors.portlet = function (context) {

  // Hide panel with form to customize panels
  $('div#portlet-categories-menu').hide();
  // Replace checkboxes by image representation
  //$('.portlet-checkbox-checked').parent().css('background', 'red');
  //$('.portlet-checkbox-unchecked').parent().css('background', 'blue');
  // Hide checkboxes when dey  replaced by "image - checkboxes"
  //$('.portlet-checkbox').hide();

  $('a#portlet-customize').click(function() {
    if ($('div#portlet-categories-menu').is(':visible')) {
      $('div#portlet-categories-menu').hide('slow');
    } else {
      $('div#portlet-categories-menu').show('slow');
    }
    return false;
  });
  
  /*
  function checkCheckbox(name) {
    $(name).attr('checked', true).removeClass('portlet-checkbox-unchecked').addClass('portlet-checkbox-checked').parent().css('background', 'red');
  }
  
  function uncheckCheckbox(name) {
    $(name).attr('checked', false).removeClass('portlet-checkbox-checked').addClass('portlet-checkbox-unchecked').parent().css('background', 'blue');
  }
  */
  
  function savePanelsPosition() {
    // @todo Actualize only ui.item(id)
    var left = String($('div#portlet-left-column').sortable('toArray'));
    var center = String($('div#portlet-center-column').sortable('toArray'));
    var right = String($('div#portlet-right-column').sortable('toArray'));

    $.post('portlet/save/ajax', { left: left, center: center, right: right });
  }

  function minimizePanel(panel) {
    var url = $(panel).children('.portlet-panel-title').children('.portlet-resize').attr('href');
    $.getJSON(url + '/ajax', function(data) {
      $(panel).children('.portlet-panel-title').children('.portlet-resize').attr('href', data.url).html('M');
      $(panel).children('.portlet-panel-content').slideUp('slow');
    });
  }

  function maximizePanel(panel) {
    var url = $(panel).children('.portlet-panel-title').children('.portlet-resize').attr('href');
    $.getJSON(url + '/ajax', function(data) {
      $(panel).children('.portlet-panel-title').children('.portlet-resize').attr('href', data.url).html('m');
      $(panel).children('.portlet-panel-content').slideDown('slow');
    });
  }

  function closePanel(panel) {
    var url = $(panel).children('.portlet-panel-title').children('.portlet-close').attr('href');
    var checkbox = '#' + $(panel).attr('id') + '-checkbox';
    $.getJSON(url + '/ajax', function(data) {
      $(panel).slideUp('slow', function() {
        uncheckCheckbox(checkbox);
        $(panel).remove();
      });
    });
  }
 

  /**
   * Function is called after
   * panel was moved to new column.
   */
  function stopMovingPanel(event, ui) {
    savePanelsPosition();
  }
  
  /**
   * Fuction is called before
   * panel was start moving
   */
  function startMovingPanel(event, ui) {
    var width = $(this).css('width');
    $(ui.item).css('width', width);
  }

  $('.portlet-handler-js').css('cursor', 'move');

  $('a.portlet-close').click(function() {
    var panel = $(this).parent().parent();
    closePanel(panel);
    return false;
  });
  
  /**
   * Handling for checkboxes
   * Parents should react on clicking
   */
  $('.portlet-checkbox').parent().click(function () {
    if ($(this).hasClass('portlet-checkbox-checked')) {
      uncheckCheckbox(checkbox);
    } else {
      checkCheckbox(checkbox);
    }
  });

  // Service +/- links by ajax
  $('a.portlet-countup').click(function () {
    var url = $(this).attr('href');
    var bar = $(this).parent();
    var content = $(bar).next();

    $.getJSON(url + '/ajax', function(data) {
      $(bar).children('.portlet-countdown').attr('href', data.countdown);
      $(bar).children('.portlet-countup').attr('href', data.countup);
      var cid = $(content).attr('id'); 
      $(content).html(data.content);
    });

    return false;
  });

  $('a.portlet-countdown').click(function () {
    var url = $(this).attr('href');
    var bar = $(this).parent();
    var content = $(bar).next();

    $.getJSON(url + '/ajax', function(data) {
      $(bar).children('.portlet-countdown').attr('href', data.countdown);
      $(bar).children('.portlet-countup').attr('href', data.countup);
      $(content).html(data.content);
    });

    return false;
  });

  $('a.portlet-resize').click(function () {
    panel = $(this).parent().parent();
    if ($(panel).children('.portlet-panel-content').is(':visible')) {
      minimizePanel(panel);
    } else {
      maximizePanel(panel);
    }
    return false;
  });

  // Select all elements witch should be movable
  var columns = ['.portlet-column-js'];
  var $columns = $(columns.toString());

  // Make column's elements movable
  $columns.sortable({
    items: '> .portlet-panel-js',
    handle: '.portlet-handler-js',
    cursor: 'move',
    opacity: 0.5,
    scroll: true,
    scrollSensitivity: '100',
    revert: true,
    tolerance: 'pointer',
    connectWith: columns,
    // @todo somehow add resize animation after drop
    // change CSS removing 100% weight for childrens
    // think about it
    start: startMovingPanel,
    stop: stopMovingPanel,
  });

  //Make accordion nodes in panels
  $('ul.portlet-node-list').accordion({
    headerSelector: 'a.portlet-node-title',
    event: 'mouseover',
    autoHeight: false,
    clearStyle: true
  });
};
