<?php

if (!function_exists("huoltosykli")) {

	function huoltosyklitarkista(&$t, $i, $result, $tunnus, &$virhe, $trow) {
		global $kukarow, $yhtiorow, $alias_set, $alasveto, $asiakasid;

		//t�m�n avulla tarkistetaan, ett� kantaan ei mene kahta huoltosykli� samoilla atribuuteilla
		//merkitsevi� atribuutteja on laitteen koko, paikan olosuhde ja laitteen tyyppi
		if (mysql_field_name($result, $i) == "tunnus") {
			$query = "	SELECT koko,
						olosuhde,
						tyyppi
						FROM huoltosykli
						WHERE yhtio = '{$kukarow['yhtio']}'
						AND koko = '{$t['koko']}'
						AND olosuhde = '{$t['olosuhde']}'
						AND tyyppi = '{$t['tyyppi']}'";
			$vanha_huoltosykli_result = pupe_query($query);

			if (mysql_num_rows($vanha_huoltosykli_result) > 0) {
				//t�m� on sit� varten, ett� virhe tilanteessa ei pystyt� luomaan uutta
				$virhe[$i] .= t("Huoltosykli on jo perustettu");

				//t�m� on sit� varten, ett� virheilmoitus tulee n�kyviin
				$virhe[1] .= t("Huoltosykli on jo perustettu");
			}
		}

		//tyyppi pit�� l�yty� tuotteen_avainsanat taulusta
		if (mysql_field_name($result, $i) == "tyyppi") {
			if (empty($t[$i])) {
				$virhe[$i] = t("Tyyppi ei voi olla tyhj�");
			}
			else {
				$query = "	SELECT *
							FROM tuotteen_avainsanat
							WHERE yhtio = '{$kukarow['yhtio']}'
							AND selite = '{$t[$i]}'";
				$tyyppi_result = pupe_query($query);

				if (mysql_num_rows($tyyppi_result) < 0) {
					$virhe[$i] = t("Tuoteen tyyppi ei ole perustettu");
				}
			}
		}

		if (mysql_field_name($result, $i) == "koko") {
			if (empty($t[$i])) {
				$virhe[$i] = t("Koko ei voi olla tyhj�");
			}
		}

//olosuhde pit�� l�yty� avainsanat taulusta OLOSUHDE
		if (mysql_field_name($result, $i) == "olosuhde") {
			$query = "	SELECT *
						FROM avainsana
						WHERE yhtio = '{$kukarow['yhtio']}'
						AND laji = 'OLOSUHDE'
						AND selite = '{$t[$i]}'";
			$tyyppi_result = pupe_query($query);

			if (mysql_num_rows($tyyppi_result) < 0) {
				$virhe[$i] = t("Olosuhdetta ei ole perustettu");
			}
		}

		if (mysql_field_name($result, $i) == "toimenpide") {
			$query = "	SELECT tunnus
						FROM tuote
						WHERE yhtio = '{$kukarow['yhtio']}'
						AND tuoteno = '{$t[$i]}'";
			$toimenpide_result = pupe_query($query);

			if (mysql_num_rows($toimenpide_result) < 0) {
				$virhe[$i] = t("Virheellinen toimenpide").". ".t("Toimenpide pit�� olla tuote");
			}
		}

//huoltov�li float aluksi annetaan p�iviss�, my�hemmin voi olla mittarilukema, k�ytt�kerta
		if (mysql_field_name($result, $i) == "huoltovali") {
			if (!is_numeric($t[$i])) {
				$virhe[$i] = t("Huoltov�li annettava p�ivin�");
				//$virhe[$i] = t("Huoltov�li annettava p�ivin�, mittarilukemana tai k�ytt�kertana");
			}

			if (empty($t[$i])) {
				$virhe[$i] = t("Huoltov�li ei voi olla tyhj�!");
			}
		}

		if (mysql_field_name($result, $i) == "pakollisuus") {
			if ((int)$t[$i] != 1 and (int)$t[$i] != 0) {
				$virhe[$i] = t("Virheellinen pakollisuus sy�tetty");
			}
		}
	}
}