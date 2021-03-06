<?php
$feature = $variables['node']->feature;
$haplotypes = $feature->haplotypes;

if (count($haplotypes) > 0) { ?>
  <div id="tripal_feature-haplotypes-box" class="tripal_feature-info-box tripal-info-box">
    <table id="tripal_feature-haplotypes-table" class="tripal_feature-table tripal-table tripal-table-horz" style="width:100%;">
      <tr><th>Marker</th>
          <?php 
            $count = 1;
            foreach($haplotypes as $marker_feature_id => $haplotype) {
              $arr = $haplotype->haplotypes;
              ksort($arr);
              if ($count == 1) {
                foreach ($arr AS $k => $v) {
                  print "<th>$k</th>";
                }
                print "</tr>";
              }
              $class = "even";
              if ($count % 2 == 1) {
                $class = "odd";
              }
              print "<tr class=\"tripal_feature-table-$class-row $class\">";
              $link = mainlab_tripal_link_record('feature', $haplotype->feature_id);
              print "<td><a href=\"$link\">$haplotype->name</a></td>";
              foreach ($arr AS $k => $v) {
                print "<td>$v</td>";
              }
              print "</tr>";
              $count ++; 
            }
            // Add more td to ensure the width of each cell is fixed
            
        ?>
    </table>
  </div>
<?php } ?>