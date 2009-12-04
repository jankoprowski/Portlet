<div id="portlet-categories-menu">
<?php  echo drupal_get_form('portlet_setup'); ?>
</div>

<div id="portlet-link-menu">
  <a href="#" id="portlet-customize">Dostosuj</a> | <a href="<?php echo url('portlet/reset'); ?>">Zresetuj panele</a>
</div>
<div id="portlet-left-column" class="portlet-column portlet-column-js">
<?php
if (isset($columns['left'])):
  foreach ($columns['left'] as $portlet):
    extract((array)$portlet, EXTR_OVERWRITE);
    echo theme('portlet_'.$type, $pid, $title, $arg, $count, $state, $countable);
  endforeach;
endif ?>
</div>

<div id="portlet-center-column" class="portlet-column portlet-column-js">
<?php
if (isset($columns['center'])):
  foreach ($columns['center'] as $portlet):
    extract((array)$portlet, EXTR_OVERWRITE);
    echo theme('portlet_'.$type, $pid, $title, $arg, $count, $state, $countable);
  endforeach;
endif ?>
</div>

<div id="portlet-right-column" class="portlet-column portlet-column-js">
<?php
if (isset($columns['right'])):
  foreach ($columns['right'] as $portlet):
    extract((array)$portlet, EXTR_OVERWRITE);
    echo theme('portlet_'.$type, $pid, $title, $arg, $count, $state, $countable);
  endforeach;
endif ?>
</div>

