<?php

if (!function_exists("puun_alkiotarkista")) {
	function puun_alkiotarkista (&$t, $i, $result, $tunnus, &$virhe, $trow) {
		global $kukarow, $yhtiorow, $alias_set, $alasveto, $table_tarkenne;
		static $tem_laji, $liitos_ind, $tem_liitos;

		if (mysql_field_name($result, $i) == "liitos") {
			$liitos_ind = $i;
			$tem_liitos = isset($trow['liitos']) ? $trow['liitos']: $t[$i];
		}

		if (mysql_field_name($result, $i) == "laji") {
			$tem_laji = $t[$i];

			if ($t[$i] == '') {
				$virhe[$i] = t("Lajia ei voi j�tt�� tyhj�ksi")."!";
			}
		}

		if (mysql_field_name($result, $i) == "puun_tunnus") {
			if ($t[$i] == '') {
				$virhe[$i] = t("Puun tunnusta ei voi j�tt�� tyhj�ksi")."!";
			}
		}

		if ((!isset($trow["luedata_toiminto"]) or $trow["luedata_toiminto"] != "POISTA") and (strtoupper($tem_laji) == 'ASIAKAS' or strtoupper($table_tarkenne) == 'ASIAKAS') and mysql_field_name($result, $i) == "tunnus") {
			$query = "	SELECT tunnus
						FROM asiakas
						WHERE yhtio = '{$kukarow['yhtio']}'
						AND laji != 'P'
						AND tunnus = '$tem_liitos'";
			$asiakkaan_haku_res = pupe_query($query);

			if (mysql_num_rows($asiakkaan_haku_res) == 0) {
				$virhe[$i] = $virhe[$liitos_ind] = t("Asiakasta ei l�ydy")."!";
			}
		}

		if ((!isset($trow["luedata_toiminto"]) or $trow["luedata_toiminto"] != "POISTA") and (strtoupper($tem_laji) == 'TUOTE' or strtoupper($table_tarkenne) == 'TUOTE') and mysql_field_name($result, $i) == "tunnus") {
			$query = "	SELECT tunnus
						FROM tuote
						WHERE yhtio = '{$kukarow['yhtio']}'
						AND status != 'P'
						AND tuoteno = '$tem_liitos'";
			$asiakkaan_haku_res = pupe_query($query);

			if (mysql_num_rows($asiakkaan_haku_res) == 0) {
				$virhe[$i] = $virhe[$liitos_ind] = t("Tuotetta ei l�ydy")."!";
			}
		}
	}
}
