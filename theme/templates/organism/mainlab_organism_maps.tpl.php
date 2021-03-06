<?php

$organism = $variables['node']->organism;

// expand the organism object to include the feature maps and their properties
$options = array('return_array' => 1);
$organism = chado_expand_var($organism, 'table', 'featuremap_organism', $options);
$featuremap_organisms = key_exists('featuremap_organism', $organism) ? $organism->featuremap_organism : array();

if (count($featuremap_organisms) > 0) {?>
  <div id="tripal_organism-maps-box" class="tripal_organism-info-box tripal-info-box">
    <table id="tripal_organism-table-collection" class="tripal_organism-table tripal-table tripal-table-horz">     
      <tr class="tripal_organism-table-odd-row tripal-table-even-row">
        <th>Map Name</th>
        <th>Map Details</th>
      </tr> <?php
      $i = 1;
      foreach ($featuremap_organisms as $featuremap_organism){ 
        // get the ma properties
        $featuremap = $featuremap_organism->featuremap_id;
        $values = array('featuremap_id' => $featuremap->featuremap_id);
        $properties = chado_generate_var('featuremapprop', $values, $options);
        
        $class = 'tripal_organism-table-odd-row odd';
        if($i % 2 == 0){
          $class = 'tripal_organism-table-odd-row even';
        } ?>
        <tr class="<?php print $class ?>">
          <td><?php 
          $link = mainlab_tripal_link_record('featuremap', $featuremap->featuremap_id);
            if($link){
              print "<a href=\"$link\">$featuremap->name</a>";
            } 
            else {
              print $featuremap->name;
            } ?>
          </td>
          <td>
            <table class="tripal-subtable"> <?php
              foreach ($properties as $property){ ?>
                <tr>
                  <td><?php print ucwords(preg_replace('/_/', ' ', $property->type_id->name))?></td>
                  <td>:</td>
                  <td><?php print $property->value ?></td>
                </tr> <?php
              } ?>
            </table>
          </td>
        </tr> <?php
        $i++; 
      }?>  
    </table> 
  </div><?php
}
