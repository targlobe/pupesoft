<?php

if(!function_exists("hinnastotarkista")) {
	function hinnastotarkista (&$t, $i, $result, $tunnus, &$virhe, $trow) {
		global $kukarow, $yhtiorow, $alias_set;

		static $chtuoteno, $chalkupvm, $chloppupvm;

		if (mysql_field_name($result, $i) == "hinta") {
			if (trim($t[$i]) == '') {
				$virhe[$i] = t("Tieto puuttuu!");
			}
		}

		if (mysql_field_name($result, $i) == "tuoteno") {
			$chtuoteno = $t[$i];
			if ($chtuoteno != '') {
				$query = "	SELECT tuoteno
							FROM tuote
							WHERE yhtio = '$kukarow[yhtio]'
							AND tuoteno = '$t[$i]'";
				$sresult = mysql_query($query) or pupe_error($query);
			}

			if (mysql_num_rows($sresult) != 1 or $chtuoteno == "") {
				$query = "	SELECT tuoteno
							FROM tuotteen_toimittajat
							WHERE yhtio = '$kukarow[yhtio]'
							AND toim_tuoteno = '$t[$i]'";
				$sresult = mysql_query($query) or pupe_error($query);

				if (mysql_num_rows($sresult) == 0 or $chtuoteno == "") {
					$virhe[$i] = t("Tuotenumero puuttuu tai sit� ei l�ydy!");
				}
			}

		}

		if (mysql_field_name($result, $i) == "alkupvm") {
			$chalkupvm = $t[$i];
			if ($chalkupvm == '' or $chalkupvm == '0000-00-00') {
				$chalkupvm = '0000-00-00';
			}
			else {
				list($vv,$kk,$pp) = explode("-", $chalkupvm);
				if (!checkdate($kk,$pp,$vv)) {
					$virhe[$i] = t("Virheellinen p�iv�m��r�!");
				}
			}
		}

		if (mysql_field_name($result, $i) == "loppupvm") {
			$chloppupvm = $t[$i];
			if ($chloppupvm == '' or $chloppupvm == '0000-00-00') {
				$chloppupvm = '0000-00-00';
			}
			else {
				list($vv,$kk,$pp) = explode("-", $chloppupvm);
				if (!checkdate($kk,$pp,$vv)) {
					$virhe[$i] = t("Virheellinen p�iv�m��r�!");
				}
			}
		}
	}
}


?>
