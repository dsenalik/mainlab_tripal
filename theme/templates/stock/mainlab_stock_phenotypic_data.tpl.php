<?php
$stock = $variables['node']->stock;
$phenotypic_data = $stock->phenotypic_data;
$num_phenotypic_data = count($phenotypic_data);
if ($num_phenotypic_data > 0) {
?>

<?php 
// Load the table pager javascript code as we'll need it after the allele table is created.
drupal_add_js(drupal_get_path('module', 'mainlab_tripal') . "/theme/js/mainlab_table_pager.js");
?>

  <div id="tripal_stock-phenotypic_data-box" class="tripal_stock-info-box tripal-info-box">
    <div class="tripal_stock-info-box-title tripal-info-box-title">Phenotypic Data</div>
    <div style="float:left; margin-bottom: 15px;">Total <?php print $num_phenotypic_data;?> trait scores</div>
    <?php 
      $dir = 'sites/default/files/tripal/mainlab_tripal/download';
      if (!file_exists($dir)) {
        mkdir ($dir, 0777);
      }
      $download = $dir . '/phenotypic_data_stock_id_' . $stock->stock_id . '.csv';
      $handle = fopen($download, "w");
      fwrite($handle, "Phenotypic Data for Germplasm '" . $stock->uniquename. "'\n");
      fwrite($handle, '"#","Dataset","Descriptor","Value","Environment","Replication"' . "\n");
    ?>
    <div style="float: right">Download <a href="<?php print '/' . $download;?>">Table</a></div>
    <table id="tripal_stock-phenotypic_data-table" class="tripal_stock-table tripal-table tripal-table-horz" style="margin-bottom:20px;">
             <tr>
               <th>#</th>
               <th>Dataset</th>
               <th>Descriptor</th>
               <th>Value</th>
             </tr>
    <?php
      $counter = 0;
      $class = "";
      foreach ($phenotypic_data as $score){
         if ($counter % 2 == 0) {
            $class = "tripal_stock-table-even-row even";
         } else {
            $class = "tripal_stock-table-odd-row odd";
         }
         print "<tr class=\"$class\"><td>". ($counter + 1) . "</td><td>$score->project</td><td>$score->descriptor</td><td>$score->value</td></tr>";
         fwrite($handle, '"' . ($counter + 1) . '","'. $score->project . '","' . $score->descriptor . '","' . $score->value . '"' . "\n");
         $counter ++;
      }
      fclose($handle);
    ?>
    </table>
  </div>
 <?php
}
