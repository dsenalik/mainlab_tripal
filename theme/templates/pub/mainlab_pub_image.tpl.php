<?php
$pub = $variables['node']->pub;
$images = property_exists($pub, 'images') ? $pub->images : array();

//$path = file_directory_path() . '/tripal/tripal_pub/images/';
$icon_path = '';
$img_path = '';
if (mainlab_tripal_get_site() == 'cottongen') {
  $icon_path = 'sites/default/files/bulk_data/www.cottongen.org/cotton_photo/publication/icon/icon-';
  $img_path = 'sites/default/files/bulk_data/www.cottongen.org/cotton_photo/publication/image/';
}

if (count($images) > 0) { ?>
  <div id="tripal_pub-images-box" class="tripal_pub-info-box tripal-info-box">
        
    <table id="tripal_pub-images-table" class="tripal_pub-table tripal-table tripal-table-horz" style="width:700px;">
        <tr>
          <?php 
            $count = 1;
            foreach($images as $img) {
              $iconurl = url($icon_path . $img->image_uri);
              $imgurl = url($img_path . $img->image_uri);
              print "<td  style=\"width:200px;padding:20px 0px 0px 20px;\">";
              print "<a href=$imgurl target=_blank><img src=\"$iconurl\" style=\"cursor:pointer;\"></a>"; 
              print "<div style=\"clear:left;margin-bottom:10px;\">" . $img->legend ."</div></td>";
              if ($count % 3 == 0) {
                print "</tr><tr>";
              }
            $count ++; 
            }
            // Add more td to ensure the width of each cell is fixed
            $more = 3 - (count($images) % 3);
            for ($i = 0; $i < $more; $i ++) {
              print "<td  style=\"width:200px;padding:20px 0px 0px 20px;\"></td>";
              
            }
        ?>
      </tr> 
    </table>
  </div>
<?php } ?>