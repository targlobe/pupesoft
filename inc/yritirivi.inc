<?php

	$ulos = "";
	$jatko = 1; // oletetaan normaali k�sittely
	$tyyppi = 1; // oletetaan rivin n�kyvyys

	if ($i == 0) { //yhti�t� ei n�ytet�
		$tyyppi = 0;
	}

	if (mysql_field_name($result, $i) == "tilinumero") { //suoraveloitus suoritetaan automaattisesti
		$maxsize++; //Tilaa mahdolliselle viivalle
	}

	if (mysql_field_name($result, $i) == "kaytossa") {

		$sel1 = '';
		$sel2 = '';

		if (strtoupper($trow[$i]) == 'E') {
			$sel2 = "SELECTED";
		}
		else {
			$sel1 = "SELECTED";
		}

		$ulos  = "<td><select name='$nimi'>";
		$ulos .= "<option value = '' $sel1>".t("K�yt�ss�")."</option>";
		$ulos .= "<option value = 'E' $sel2>".t("Poistettu / Ei k�yt�ss�")."</option>";
		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if (mysql_field_name($result, $i) == "valkoodi") { //valuutta
		$query = "	SELECT nimi, tunnus
					FROM valuu
					WHERE yhtio = '$kukarow[yhtio]'
					ORDER BY jarjestys";
		$vresult = mysql_query($query) or pupe_error($query);

		$ulos = "<td><select name='$nimi'>";

		while ($vrow=mysql_fetch_array($vresult)) {
			$sel="";
			if ($trow[$i] == $vrow['nimi']) {
					$sel = "selected";
			}
			$ulos .= "<option value = '$vrow[nimi]' $sel>$vrow[nimi]";
		}

		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if (mysql_field_name($result, $i) == "factoring") {

		$ulos  = "<td><select name='$nimi'>";

		$chk1 = '';
		$chk2 = '';

		if ($trow[$i] == "") {
			$chk1 = 'selected';
		}
		else {
			$chk2 = 'selected';
		}

		$ulos .= "<option value=''  $chk1>".t("Ei")."</option>";
		$ulos .= "<option value='o' $chk2>".t("Kyll�")."</option>";

		$ulos .= "</select></td>";
		$jatko = 0;

	}

 	if (mysql_field_name($result, $i) == "hyvak") { //hyv�ksyj�t
		$query = "SELECT kuka, nimi
					FROM kuka
					WHERE yhtio = '$kukarow[yhtio]'
					ORDER BY nimi";
		$vresult = mysql_query($query) or pupe_error($query);
		$ulos = "<td><select name='$nimi'><option value=''>".t("Ei kukaan");

		while ($vrow=mysql_fetch_array($vresult)) {
			$sel="";

			if ($trow[$i] == $vrow[0]) {
				$sel = "selected";
			}

			$ulos .= "<option value = '$vrow[0]' $sel>$vrow[1]";
		}

		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if (mysql_field_name($result, $i) == "oletus_kustp") {
		$query = "	SELECT tunnus, nimi
					FROM kustannuspaikka
					WHERE yhtio = '$kukarow[yhtio]'
					and kaytossa != 'E'
					and tyyppi = 'K'
					ORDER BY koodi+0, koodi, nimi";
		$vresult = mysql_query($query) or pupe_error($query);
		$ulos = "<td><select name='$nimi'><option value=' '>".t("Ei kustannuspaikkaa")."</option>";

		while ($vrow=mysql_fetch_array($vresult)) {
			$sel="";
			if ($trow[$i] == $vrow[0]) {
				$sel = "selected";
			}
			$ulos .= "<option value = '$vrow[0]' $sel>$vrow[1]</option>";
		}
		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if (mysql_field_name($result, $i) == "oletus_kohde") {
		$query = "	SELECT tunnus, nimi
					FROM kustannuspaikka
					WHERE yhtio = '$kukarow[yhtio]'
					and kaytossa != 'E'
					and tyyppi = 'O'
					ORDER BY koodi+0, koodi, nimi";
		$vresult = mysql_query($query) or pupe_error($query);
		$ulos = "<td><select name='$nimi'><option value=' '>".t("Ei kohdetta")."</option>";

		while ($vrow=mysql_fetch_array($vresult)) {
			$sel="";
			if ($trow[$i] == $vrow[0]) {
				$sel = "selected";
			}
			$ulos .= "<option value = '$vrow[0]' $sel>$vrow[1]</option>";
		}
		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if (mysql_field_name($result, $i) == "oletus_projekti") {
		$query = "	SELECT tunnus, nimi
					FROM kustannuspaikka
					WHERE yhtio = '$kukarow[yhtio]'
					and kaytossa != 'E'
					and tyyppi = 'P'
					ORDER BY koodi+0, koodi, nimi";
		$vresult = mysql_query($query) or pupe_error($query);
		$ulos = "<td><select name='$nimi'><option value=' '>".t("Ei projektia")."</option>";

		while ($vrow=mysql_fetch_array($vresult)) {
			$sel="";
			if ($trow[$i] == $vrow[0]) {
				$sel = "selected";
			}
			$ulos .= "<option value = '$vrow[0]' $sel>$vrow[1]</option>";
		}
		$ulos .= "</select></td>";
		$jatko = 0;
	}

	if	(mysql_field_name($result, $i) == "oletus_kulutili" or
		mysql_field_name($result, $i) == "oletus_selvittelytili" or
		mysql_field_name($result, $i) == "oletus_rahatili") {

		if ($ulosarray[$i] != '') {
			$ulos=$ulosarray[$i];
			$jatko = 0;
		}
		else {
			$ulos = "<td><input type = 'text' name = '$nimi' value = '$trow[$i]' size='$size' maxlength='" . mysql_field_len($result,$i) ."'>";
			$query = "	SELECT nimi
						FROM tili
						WHERE yhtio='$kukarow[yhtio]' and tilino = '$trow[$i]'";
			$vresult = mysql_query($query) or pupe_error($query);

			if (mysql_num_rows($vresult) == 1) {
				$vrow = mysql_fetch_array($vresult);
				$ulos .= " $vrow[nimi]</td>";
				$jatko = 0;
			}
		}
	}

	if	(mysql_field_name($result, $i) == "tilinylitys") {
		$ulos  = "<td><select name='$nimi'>";

		$chk1 = '';
		$chk2 = '';

		if ($trow[$i] == "") {
			$chk1 = 'selected';
		}
		else {
			$chk2 = 'selected';
		}

		$ulos .= "<option value=''  $chk1>".t("Tilinylitys ei sallittu")."</option>";
		$ulos .= "<option value='o' $chk2>".t("Tilinylitys sallittu")."</option>";

		$ulos .= "</select></td>";
		$jatko = 0;
	}


	if (mysql_field_name($result, $i) == "siirtoavain" or
		mysql_field_name($result, $i) == "kasukupolvi" or
		mysql_field_name($result, $i) == "kayttoavain" or
		mysql_field_name($result, $i) == "nro" or
		mysql_field_name($result, $i) == "sasukupolvi" or
		mysql_field_name($result, $i) == "kertaavain" or
		mysql_field_name($result, $i) == "siemen" or
		mysql_field_name($result, $i) == "salattukerta" or
		mysql_field_name($result, $i) == "generointiavain" or
		mysql_field_name($result, $i) == "asiakas" or
		mysql_field_name($result, $i) == "pankki" or
		mysql_field_name($result, $i) == "asiakastarkenne" or
		mysql_field_name($result, $i) == "pankkitarkenne") {
			$tyyppi = 0;
	}
?>
