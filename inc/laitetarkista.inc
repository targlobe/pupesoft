<?php

if (!function_exists("laitetarkista")) {

	function laitetarkista(&$t, $i, $result, $tunnus, &$virhe, $trow) {
		global $kukarow, $yhtiorow, $alias_set, $alasveto, $asiakasid;

		//laitetta ei saa perustaa jos sen sammutin tuotteelle (tuote-taulu) ei ole linkattu koko ja tyyppi�, koska t�ll�in ty�m��r�ysten generointi ei toimi
		if (mysql_field_name($result, $i) == "tunnus") {
			$query = "	SELECT DISTINCT laji
						FROM tuotteen_avainsanat
						WHERE yhtio = '{$kukarow['yhtio']}'
						AND tuoteno = '{$t['tuoteno']}'";
			$tuotteen_atribuutit_result = pupe_query($query);

			if (mysql_num_rows($tuotteen_atribuutit_result) != 2) {
				$virhe[$i] = t("Tuotteen avainsanat on perustamatta").". ".t("Tarvittuja avainsanoja on koko ja tyyppi");

				$virhe[1] .= t("Tuotteen avainsanat on perustamatta").". ".t("Tarvittuja avainsanoja on koko ja tyyppi");
			}
		}

		if (mysql_field_name($result, $i) == "paikka") {
			$tunnus = trim($t[$i]);

			if (!empty($tunnus)) {
				$query = "	SELECT *
							FROM paikka
							WHERE yhtio = '{$kukarow['yhtio']}'
							AND tunnus = '{$tunnus}'";
				$kohde_result = pupe_query($query);

				if (mysql_num_rows($kohde_result) != 1) {
					$virhe[$i] = t("Paikka puuttuu tai sit� ei l�ydy!");
				}
			}
		}

		if (mysql_field_name($result, $i) == "tuoteno") {
			$tuoteno = trim($t[$i]);

			if (!empty($tuoteno)) {
				$query = "	SELECT *
							FROM tuote
							WHERE yhtio = '{$kukarow['yhtio']}'
							AND tuoteno = '{$tuoteno}'";
				$tuote_result = pupe_query($query);

				if (mysql_num_rows($tuote_result) != 1) {
					$virhe[$i] = t("Tuote puuttuu tai sit� ei l�ydy!");
				}
			}
		}
	}
}