<?php
$stock = $variables['node']->stock;
$genotypic_data = $stock->genotypic_data;
$num_genotypic_data = count($genotypic_data);
if ($num_genotypic_data > 0) {
?>

<?php 
// Load the table pager javascript code as we'll need it after the allele table is created.
drupal_add_js(drupal_get_path('module', 'mainlab_tripal') . "/theme/js/mainlab_table_pager.js");
?>

<script type="text/javascript">
// Insert Genotypic data value to the base template
$('#tripal_stock-table-genotypic_data_value').html("[<a href='#' id='tripal_stock-table-genotypic_data_value-link'>view all <?php print $num_genotypic_data;?></a>]");
$('#tripal_stock-table-genotypic_data_value-link').click(function() {
  $('.tripal-info-box').hide();
  $('#tripal_stock-genotypic_data-box').fadeIn('slow');
  $('#tripal_stock_toc').height($('#tripal_stock-genotypic_data-box').parent().height());
})
</script>

  <div id="tripal_stock-genotypic_data-box" class="tripal_stock-info-box tripal-info-box">
    <div class="tripal_stock-info-box-title tripal-info-box-title">Genotypic Data</div>
       <div style="float:left; margin-bottom: 15px;">Total <?php print $num_genotypic_data;?> genotypic data</div>
       <?php
        $dir = 'sites/default/files/tripal/mainlab_tripal/download';
        if (!file_exists($dir)) {
          mkdir ($dir, 0777);
        }
        $download = $dir . '/genotypic_data_stock_id_' . $stock->stock_id . '.csv';
        $handle = fopen($download, "w");
        fwrite($handle, "Genotypic Data for Germplasm '" . $stock->uniquename. "'\n");
        fwrite($handle, '"#","Dataset","Marker","Marker Type","Genotype","Marker_Allele"' . "\n");
    ?>
    <div style="float: right">Download <a href="<?php print '/' . $download;?>">Table</a></div>
    <table id="tripal_stock-genotypic_data-table" class="tripal_stock-table tripal-table tripal-table-horz" style="margin-bottom:20px;">
             <tr>
             <th>#</th>
               <th>Dataset</th>
               <th>Marker</th>
               <th>Marker Type</th>
               <th>Genotype</th>
               <th>Marker_Allele</th>
             </tr>
    <?php
      $counter = 0;
      $class = "";
      foreach ($genotypic_data as $data){
         if ($counter % 2 == 0) {
            $class = "tripal_stock-table-even-row even";
         } else {
            $class = "tripal_stock-table-odd-row odd";
         }
         $descriptor = explode("_", $data->uniquename);
         $marker = $descriptor[0];
         for ($i = 1; $i < count($descriptor) - 1; $i ++) {
            $marker .= '_' . $descriptor[$i];
         }
         $gtype = $descriptor[count($descriptor) - 1];
         $alleles = explode("|", $gtype );
         $link_alleles = "";
         $alleles_wo_link = "";
         $index = 0;
         foreach($alleles AS $allele) {
            $link_alleles .= "<a href=\"/allele/$marker/$allele/$data->organism_id\">" .$marker ."_" . $allele . "</a>";
            $alleles_wo_link .= $marker ."_" . $allele;
            if ($index < count($alleles) - 1) {
               $link_alleles .= "; ";
               $alleles_wo_link .= "; ";
            }
            $index ++;
         }
         $link = mainlab_tripal_link_record('feature', $data->feature_id);
         print "<tr class=\"$class\"><td>". ($counter + 1) . "</td><td>$data->project</td><td><a href=\"$link\">$marker</a></td><td>$data->marker_type</td><td>$descriptor[1]</td><td>$link_alleles</td></tr>";
         global $base_url;
         fwrite($handle, '"' . ($counter + 1) . '","'. $data->project . '","=HYPERLINK(""' . $base_url . $link . '"",""' . $marker . '"")","' . $data->marker_type . '","' . $descriptor[1] . '","'. $alleles_wo_link . '"' . "\n");
         $counter ++;
      }
      fclose($handle);
    ?>
    </table>
  </div>
 <?php
}
