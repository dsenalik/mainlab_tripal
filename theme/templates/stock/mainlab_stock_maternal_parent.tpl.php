<?php
$stock = $variables['node']->stock;
$maternal_parent = $stock->maternal_parent;
$num_mparent = count($maternal_parent);
if ($num_mparent > 0) {
  $first_mparent = $maternal_parent[0]->uniquename;
  if ($num_mparent > 1) {
    $first_mparent .= " [<a href=\"#\" id=\"tripal_stock-table-maternal_parent_value-link\">view all " . $num_mparent . "</a>]";
  }
?>
<script type="text/javascript">
// Insert Maternal Parent value to the base template
$('#tripal_stock-table-maternal_parent_value').html('<?php print $first_mparent;?>');
$('#tripal_stock-table-maternal_parent_value-link').click(function() {
  $('.tripal-info-box').hide();
  $('#tripal_stock-maternal_parent-box').fadeIn('slow');
  $('#tripal_stock_toc').height($('#tripal_stock-maternal_parent-box').parent().height());
})
</script>

  <div id="tripal_stock-maternal_parent-box" class="tripal_stock-info-box tripal-info-box">
    <table id="tripal_stock-maternal_parent-table" class="tripal_stock-table tripal-table tripal-table-horz">
             <tr>
               <th>Germplasm Name</th>
               <th>Description</th>
               <th>Type</th>
             </tr>
    <?php
      $counter = 1;
      $class = "";
      foreach ($maternal_parent as $parent){
         if ($counter % 2 == 0) {
            $class = "tripal_stock-table-even-row even";
         } else {
            $class = "tripal_stock-table-odd-row odd";
         }
         $link = mainlab_tripal_link_record('stock', $parent->stock_id);
         print "<tr class=\"$class\"><td><a href=\"$link\">$parent->uniquename</a></td><td>$parent->description</td><td>$parent->type</td></tr>";
         $counter ++;
      }
    ?>
    </table>
  </div> <?php
}
