<?php

function mainlab_tripal_generate_nd_geolocation_var ($node) {
  $nd_geolocation_id = chado_get_id_from_nid('nd_geolocation', $node->nid);
  $values = array('nd_geolocation_id' => $nd_geolocation_id);
  $nd_geolocation = tripal_core_generate_chado_var('nd_geolocation', $values);
  // expand the nd_geolocation to include the properties.
  $nd_geolocation = chado_expand_var($nd_geolocation,'table', 'nd_geolocationprop', array('return_array' => 1));
  $sql =
  "SELECT nid, name, description FROM {project} P
       LEFT JOIN public.chado_project CP ON CP.project_id = P.project_id
       WHERE P.project_id IN
       (SELECT project_id
        FROM {nd_geolocation} NG
        INNER JOIN {nd_experiment} NE on NE.nd_geolocation_id = NG.nd_geolocation_id
        INNER JOIN {nd_experiment_project} NP ON NE.nd_experiment_id = NP.nd_experiment_id
        WHERE NG.nd_geolocation_id = :nd_geolocation_id
        GROUP BY project_id)";
  $results = chado_query($sql, array(':nd_geolocation_id' => $nd_geolocation_id));
  $projects = array();
  while ($d = $results->fetchObject()) {
    array_push($projects, $d);
  }
  $nd_geolocation->projects = $projects;
  $node->nd_geolocation = $nd_geolocation;
  return $node;
}

/**
 * @ingroup tripal_stock
 */
function mainlab_tripal_generate_stock_in_collection_var($node) {
  $stock = $node->stock;
  $results = chado_query(
      "SELECT
			max(SC.name) AS collection,
			max(db.name) AS db,
			max(db.urlprefix) AS urlprefix,
			string_agg(X.accession, '; ') AS accession
			FROM {stockcollection} SC
			INNER JOIN {stockcollection_stock} SS ON SS.stockcollection_id = SC.stockcollection_id
			INNER JOIN {stockcollection_db} SD ON SC.stockcollection_id = SD.stockcollection_id
			INNER JOIN {db} ON db.db_id = SD.db_id
			INNER JOIN {stock_dbxref} SX ON SX.stock_id = SS.stock_id
			INNER JOIN {dbxref} X ON SX.dbxref_id = X.dbxref_id AND X.db_id = db.db_id
			WHERE SS.stock_id = :stock_id
			GROUP BY SC.stockcollection_id
			",  array(':stock_id' => $stock->stock_id));
  $coll = NULL;
  $counter = 0;
  while ($c = $results->fetchObject()) {
    $coll[$counter] = $c;
    $counter ++;
  }
  $stock->in_collection = $coll;
  $node->stock = $stock;
  return $node;
}

function mainlab_tripal_generate_stock_parents_var ($node) {
  $stock = $node->stock;
  // Get Maternal Parent
  $results = chado_query(
      "SELECT S2.stock_id, S2.uniquename, S2.description, V.name AS type
			 FROM {stock} S1
         INNER JOIN {stock_relationship} SR ON SR.subject_id = S1.stock_id
         INNER JOIN {stock} S2 ON S2.stock_id = SR.object_id
         INNER JOIN {cvterm} V ON V.cvterm_id = S2.type_id
         WHERE SR.type_id =
         (SELECT cvterm_id
          FROM {cvterm}
          WHERE name = 'is_a_maternal_parent_of' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'MAIN'))
          AND S1.stock_id = :stock_id",  array(':stock_id' => $stock->stock_id));
  $maternal_parent = NULL;
  $counter = 0;
  while ($parent = $results->fetchObject()) {
    if (db_table_exists("chado_stock")) {
      $parent->nid = db_query("SELECT nid FROM chado_stock WHERE stock_id = :stock_id", array(':stock_id' => $parent->stock_id))->fetchField();
    }
    $maternal_parent[$counter] = $parent;
    $counter ++;
  }
  $stock->maternal_parent = $maternal_parent;
  // Get Paternal Parent
  $results = chado_query(
      "SELECT S2.stock_id, S2.uniquename, S2.description, V.name AS type
			 FROM {stock} S1
         INNER JOIN {stock_relationship} SR ON SR.subject_id = S1.stock_id
         INNER JOIN {stock} S2 ON S2.stock_id = SR.object_id
         INNER JOIN {cvterm} V ON V.cvterm_id = S2.type_id
         WHERE SR.type_id =
         (SELECT cvterm_id
          FROM {cvterm}
          WHERE name = 'is_a_paternal_parent_of' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'MAIN'))
          AND S1.stock_id = :stock_id", array(':stock_id' => $stock->stock_id));
  $paternal_parent = NULL;
  $counter = 0;
  while ($parent = $results->fetchObject()) {
    if (db_table_exists("chado_stock")) {
      $parent->nid = db_query("SELECT nid FROM chado_stock WHERE stock_id = :stock_id", array(':stock_id' => $parent->stock_id))->fetchField();
    }
    $paternal_parent[$counter] = $parent;
    $counter ++;
  }
  $stock->paternal_parent = $paternal_parent;

  // Get maps for this stock and maps of its children
  $results = chado_query(
      "SELECT
           M.featuremap_id,
			  M.name
           FROM {featuremap} M
           INNER JOIN {featuremap_stock} MS ON M.featuremap_id = MS.featuremap_id
           WHERE MS.stock_id = :stock_id1
           OR MS.stock_id in
			     (SELECT SR.object_id FROM {stock} S1
               INNER JOIN {stock_relationship} SR ON S1.stock_id = SR.subject_id
               WHERE SR.type_id IN
                  (SELECT cvterm_id
                   FROM {cvterm}
                   WHERE (name = 'is_a_maternal_parent_of'
                   OR name = 'is_a_paternal_parent_of') AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'MAIN'))
               AND S1.stock_id = :stock_id2)",  array(':stock_id1' => $stock->stock_id,  ':stock_id2' => $stock->stock_id));
  $maps = NULL;
  $counter = 0;
  while ($map = $results->fetchObject()) {
    if (db_table_exists("chado_featuremap")) {
      $map->nid = db_query("SELECT nid FROM chado_featuremap WHERE featuremap_id = :featuremap_id", array(':featuremap_id' => $map->featuremap_id))->fetchField();
    }
    $map->ptype = chado_query("SELECT value FROM {featuremapprop} WHERE type_id = (SELECT cvterm_id FROM {cvterm} WHERE name = 'population_type' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'MAIN')) AND featuremap_id = :featuremap_id", array(':featuremap_id' => $map->featuremap_id))->fetchField();
    $map->ggroup = chado_query("SELECT value FROM {featuremapprop} WHERE type_id = (SELECT cvterm_id FROM {cvterm} WHERE name = 'genome_group' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'MAIN')) AND featuremap_id =  :featuremap_id", array(':featuremap_id' => $map->featuremap_id))->fetchField();
    $map->mtype = chado_query("SELECT value FROM {featuremapprop} WHERE type_id = (SELECT cvterm_id FROM {cvterm} WHERE name = 'map_type' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'MAIN')) AND featuremap_id =  :featuremap_id", array(':featuremap_id' => $map->featuremap_id))->fetchField();

    $maps[$counter] = $map;
    $counter ++;
  }
  $stock->population_map = $maps;
  $node->stock = $stock;
  return $node;
}


/**
 * @ingroup tripal_stock
 */
function mainlab_tripal_generate_tripal_stock_phenotypic_data_var($node) {
  $stock = $node->stock;
  $results = chado_query(
      "SELECT
			P.uniquename, P.value, J.name AS project, GEO.environment, SP.value AS replications
			FROM {nd_experiment_phenotype} NEP
			INNER JOIN {phenotype} P ON NEP.phenotype_id = P.phenotype_id
			INNER JOIN {nd_experiment_stock} NES ON NES.nd_experiment_id = NEP.nd_experiment_id
			INNER JOIN {nd_experiment_project} NEJ ON NES.nd_experiment_id = NEJ.nd_experiment_id
			INNER JOIN {project} J ON J.project_id = NEJ.project_id
			INNER JOIN {nd_experiment} NE ON NE.nd_experiment_id = NEP.nd_experiment_id
			LEFT JOIN
			(SELECT
			   NG.nd_geolocation_id,
	           NGP.value AS environment
			 FROM {nd_geolocation} NG
			 INNER JOIN {nd_geolocationprop} NGP ON NG.nd_geolocation_id = NGP.nd_geolocation_id
			 AND NGP.type_id = (SELECT cvterm_id FROM {cvterm} WHERE name = 'site_code' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'MAIN'))
			) GEO ON GEO.nd_geolocation_id = NE.nd_geolocation_id
	        LEFT JOIN {stockprop} SP ON NES.stock_id = SP.stock_id AND SP.type_id = (SELECT cvterm_id FROM {cvterm} WHERE name = 'rep' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'MAIN'))
			WHERE NES.stock_id = :stock_id OR NES.stock_id IN (SELECT subject_id FROM {stock_relationship} WHERE object_id = :object_id
			AND type_id = (SELECT cvterm_id FROM {cvterm} WHERE name = 'sample_of' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'MAIN')))
	        ORDER BY project, environment, replications, uniquename", array(':stock_id' => $stock->stock_id, ':object_id' => $stock->stock_id));
  $phenotypic_data = NULL;
  $counter = 0;
  while ($traits = $results->fetchObject()) {
    $phenotypic_data[$counter] = $traits;
    $counter ++;
  }
  $stock->phenotypic_data = $phenotypic_data;
  $node->stock = $stock;
  return $node;
}

/**
 * @ingroup tripal_stock
 */
function mainlab_tripal_generate_tripal_stock_genotypic_data_var($node) {
  $stock = $node->stock;
  $results = chado_query(
      "SELECT
			G.uniquename, G.description, J.name AS project, F.organism_id, (SELECT nid FROM chado_feature WHERE feature_id = F.feature_id) AS marker_nid
			FROM {nd_experiment_genotype} NEG
			INNER JOIN {genotype} G ON NEG.genotype_id = G.genotype_id
			INNER JOIN {feature_genotype} FG ON G.genotype_id = FG.genotype_id
			INNER JOIN {feature} F ON F.feature_id = FG.feature_id
			INNER JOIN {nd_experiment_stock} NES ON NES.nd_experiment_id = NEG.nd_experiment_id
			INNER JOIN {nd_experiment_project} NEJ ON NES.nd_experiment_id = NEJ.nd_experiment_id
			INNER JOIN {project} J ON J.project_id = NEJ.project_id
			WHERE stock_id IN
			  (SELECT stock_id
			   FROM {stock} S
			   INNER JOIN {stock_relationship} SR ON S.stock_id = SR.subject_id
			   WHERE SR.type_id =
			      (SELECT cvterm_id FROM {cvterm} WHERE name = 'sample_of' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'MAIN'))
			   AND SR.object_id = :object_id)",  array(':object_id' => $stock->stock_id));
  $gdata = NULL;
  $counter = 0;
  while ($gtype = $results->fetchObject()) {
    $gdata[$counter] = $gtype;
    $counter ++;
  }
  $stock->genotypic_data = $gdata;
  $node->stock = $stock;
  return $node;
}

/**
 * @ingroup tripal_stock
 */
function mainlab_tripal_generate_stock_image_var($node) {
  $stock = $node->stock;
  // We want to show maps for this stock and maps of its children
  if (mainlab_tripal_get_site() == 'cottongen') { // CottonGen has image legend stored
    $results = chado_query(
        "SELECT eimage_type, image_uri, value as legend
			 FROM {stock_image} SI
			 INNER JOIN {eimage} I ON SI.eimage_id = I.eimage_id
			 INNER JOIN {eimageprop} IP ON I.eimage_id = IP.eimage_id
			 WHERE IP.type_id = (SELECT cvterm_id
			                                      FROM {cvterm}
			                                      WHERE name = 'legend'
			                                      AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'MAIN')
			                                    )
			AND stock_id = :stock_id",
        array(':stock_id' => $stock->stock_id));
  } else { // Do not pull legends by default (e.g. for GDR)
    $results = chado_query(
        "SELECT eimage_type, image_uri
			 FROM {stock_image} SI
			 INNER JOIN {eimage} I ON SI.eimage_id = I.eimage_id
			 WHERE stock_id = :stock_id",
        array(':stock_id' => $stock->stock_id));
  }
  $images = NULL;
  $counter = 0;
  while ($img = $results->fetchObject()) {
    $images[$counter] = $img;
    $counter ++;
  }
  $stock->images = $images;
  $node->stock = $stock;
  return $node;
}

function mainlab_tripal_generate_organism_relationships_var($node) {
  // we want to provide a new variable that contains the matched organisms.
  $organism = $node->organism;

  // normally we would use chado_expand_vars to expand our
  // organism object and add in the relationships, however whan a large
  // number of relationships are present this significantly slows the
  // query, therefore we will manually perform the query
  $sql = "
    SELECT O.genus, O.species, O.organism_id, CO.nid, CVT.name as rel_type
    FROM {organism_relationship} ORel
      INNER JOIN {organism} O on ORel.object_organism_id = O.organism_id
      INNER JOIN {cvterm} CVT on ORel.type_id = CVT.cvterm_id
      LEFT JOIN chado_organism CO on O.organism_id = CO.organism_id
    WHERE ORel.subject_organism_id = :organism_id
  ";
  $as_subject = chado_query($sql,array(':organism_id' => $organism->organism_id));
  $sql = "
    SELECT O.genus, O.species, O.organism_id, CO.nid, CVT.name as rel_type
    FROM {organism_relationship} ORel
      INNER JOIN {organism} O on ORel.subject_organism_id = O.organism_id
      INNER JOIN {cvterm} CVT on ORel.type_id = CVT.cvterm_id
      LEFT JOIN chado_organism CO on O.organism_id = CO.organism_id
    WHERE ORel.object_organism_id = :organism_id
  ";
  $as_object = chado_query($sql,array(':organism_id' => $organism->organism_id));

  // combine both object and subject relationshisp into a single array
  $relationships = array();
  $relationships['object'] = array();
  $relationships['subject'] = array();

  // iterate through the object relationships
  while ($relationship = $as_object->fetchObject()) {

    // get the relationship and child types
    $rel_type = t(preg_replace('/_/'," ",$relationship->rel_type));

    if (!array_key_exists($rel_type, $relationships['object'])) {
      $relationships['object'][$rel_type] = array();
    }
    $relationships['object'][$rel_type][] = $relationship;
  }

  while ($relationship = $as_subject->fetchObject()) {

    // get the relationship and child types
    $rel_type = t(preg_replace('/_/'," ",$relationship->rel_type));

    if (!array_key_exists($rel_type, $relationships['subject'])) {
      $relationships['subject'][$rel_type] = array();
    }
    $relationships['subject'][$rel_type][] = $relationship;
  }

  $organism->all_relationships = $relationships;
  $node->organism =$organism;
  return $node;
}

function mainlab_tripal_generate_featurepos_var ($featuremap_id) {
  $sql = "
      SELECT 
        F.uniquename AS LG, 
        (SELECT object_id FROM {feature_relationship} WHERE subject_id = F2.feature_id AND type_id = (SELECT cvterm_id FROM cvterm WHERE name = 'instance_of' AND cv_id = (SELECT cv_id FROM cv WHERE name = 'relationship'))) AS feature_id, 
        F2.uniquename AS marker, 
        FPP.value AS position 
      FROM {featurepos} FP 
      INNER JOIN {featuremap} FM ON FM.featuremap_id = FP.featuremap_id
      INNER JOIN {feature} F ON F.feature_id = FP.map_feature_id
      INNER JOIN {feature} F2 ON F2.feature_id = FP.feature_id
      INNER JOIN {featureposprop} FPP ON FP.featurepos_id = FPP.featurepos_id AND FPP.type_id = (SELECT cvterm_id FROM {cvterm} WHERE name = 'start' AND cv_id = (SELECT cv_id FROM {cv} WHERE name = 'MAIN'))
      WHERE FM.featuremap_id = :featuremap_id";
  $results = chado_query ($sql, array(':featuremap_id' => $featuremap_id));
  $positions = array();
  while ($pos = $results->fetchObject()) {
    $positions [] = $pos;
  }
  return $positions;
}