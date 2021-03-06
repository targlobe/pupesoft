<?php

	// tarvitaan $tapa jossa on vienti / tuonti
	// ja $row jossa on kentt� kaikkitunnukset ja kaikkituotteet
	// kasvatetaan $virhe muuttujaa jos l�ytyy virheit�
	// itse virheet tulee $virhetxt muuttujaan

	$virhetxt = "";
	$specutxt = "";

	$tuotteet = explode(",", $row['kaikkituotteet']);

	if ($row["tullinimike1"] == '' or $row["tullinimike1"] == 0) {
	  	foreach ($tuotteet as $tuotetiedot) {
			list($tuotetunnus, $tuoteno) = explode("!�!", $tuotetiedot);

			$virhetxt .= t("Tuote").": <a href='yllapito.php?toim=tuote&tunnus=$tuotetunnus&lopetus=$lopetus_intra1$lopetus_intra2'>$tuoteno</a>. ".t("Tullinimike tuotteelta puuttuu")."!<br>";
			$virhe++;
		}
	}
	elseif ($row["tullinimike1"] != "") {
		$query = "	SELECT cn
					FROM tullinimike
					WHERE cn = '$row[tullinimike1]'
					AND kieli = '$yhtiorow[kieli]'
					AND cn != ''";
		$tulserul = mysql_query($query) or pupe_error($query);

		if (mysql_num_rows($tulserul) == 0) {

			foreach ($tuotteet as $tuotetiedot) {
				list($tuotetunnus, $tuoteno) = explode("!�!", $tuotetiedot);

		  		$virhetxt .= t("Tuote").": <a href='yllapito.php?toim=tuote&tunnus=$tuotetunnus&lopetus=$lopetus_intra1$lopetus_intra2'>$tuoteno</a> ".t("Tullinimike on virheellinen")." $row[tullinimike1]<br>";
				$virhe++;
			}
		}
	}
	else {
		foreach ($tuotteet as $tuotetiedot) {
			list($tuotetunnus, $tuoteno) = explode("!�!", $tuotetiedot);

			$virhetxt .= t("Tuote").": <a href='yllapito.php?toim=tuote&tunnus=$tuotetunnus&lopetus=$lopetus_intra1$lopetus_intra2'>$tuoteno</a> ".t("Tullinimike on virheellinen")." $row[tullinimike1]<br>";
			$virhe++;
		}
	}

	if ($row["alkuperamaa"] == '') {
		foreach ($tuotteet as $tuotetiedot) {
			list($tuotetunnus, $tuoteno) = explode("!�!", $tuotetiedot);

			$virhetxt .= t("Alkuper�maa puuttuu tuotteelta")." <a href='yllapito.php?toim=tuote&tunnus=$intrtuorow[tunnus]&lopetus=$lopetus_intra1$lopetus_intra2'>$intrtuorow[tuoteno]</a>!<br>";
			$virhe++;
		}
	}
	elseif ($row["alkuperamaa"] == $yhtiorow["maa"] and $tapa != "vienti") {
		foreach ($tuotteet as $tuotetiedot) {
			list($tuotetunnus, $tuoteno) = explode("!�!", $tuotetiedot);

			$virhetxt .= t("Tuote").": <a href='yllapito.php?toim=tuote&tunnus=$intrtuorow[tunnus]&lopetus=$lopetus_intra1$lopetus_intra2'>$intrtuorow[tuoteno]</a>. ".t("Alkuper�maa ei voi olla sama kuin yhti�n kotimaa")."!<br>";
			$virhe++;
		}
	}
	else {
		$query = "	SELECT distinct koodi
					FROM maat
					WHERE koodi = '$row[alkuperamaa]'";
		$maaresult = mysql_query($query) or pupe_error($query);

		if (mysql_num_rows($maaresult) == 0) {
			foreach ($tuotteet as $tuotetiedot) {
				list($tuotetunnus, $tuoteno) = explode("!�!", $tuotetiedot);

				$virhetxt .= t("Tuote").": <a href='yllapito.php?toim=tuote&tunnus=$intrtuorow[tunnus]&lopetus=$lopetus_intra1$lopetus_intra2'>$intrtuorow[tuoteno]</a>. ".t("Alkuper�maa on virheellinen")."!<br>";
				$virhe++;
			}
		}
	}


	// k�yd��n l�pi kaikki laskut
	$query = "	SELECT lasku.*,
				if(lasku.maa_lahetys = '', ifnull(varastopaikat.maa, lasku.yhtio_maa), lasku.maa_lahetys) maalahetys,
				if(lasku.maa_maara = '', lasku.toim_maa, lasku.maa_maara) maamaara
	 			FROM lasku
				LEFT JOIN varastopaikat ON (varastopaikat.yhtio = lasku.yhtio and varastopaikat.tunnus = lasku.varasto)
				WHERE lasku.yhtio = '$kukarow[yhtio]'
				AND lasku.tunnus in ($row[kaikkitunnukset])";
	$intrlasres = mysql_query($query) or pupe_error($query);

	while ($intrlasrow = mysql_fetch_array($intrlasres)) {

		$specutxt = "";

		// erikoiskeissi jos kyseess� on saapuminen joka on vienti�
		if ($tapa == "vienti" and $row["tapa"] == "Saapuminen") {
			$specutxt = "T�m� on saapuminen joka on viety kotimaasta ulkomaanvarastoon.<br>";
			$tapa = "vientituonti";
		}

		// erikoiskeissi jos kyseess� on lasku joka on tuontia
		if ($tapa == "tuonti" and $row["tapa"] == "Lasku") {
			$specutxt  = "T�m� on myyntilasku joka on myyty ulkomaanvarastosta kotimaahan.<br>";
			$tapa = "tuontivienti";
		}

		// echoillaan t�ll�st� infoa
		if ($tapa == "vienti" and $row["tapa"] == "Siirtolista") {
			$specutxt = "T�m� on siirtolista joka on viety kotimaasta ulkomaanvarastoon.<br>";
		}

		// echoillaan t�ll�st� infoa
		if ($tapa == "tuonti" and $row["tapa"] == "Siirtolista") {
			$specutxt  = "T�m� on siirtolista joka on tuotu ulkomaanvarastosta kotimaahan.<br>";
		}

		// kauniiiimpaa
		if ($row["tapa"] == "Siirtolista") {
			$intrlasrow["laskunro"] = $intrlasrow["tunnus"];
		}

		// tuontispecific tarkistuksia
		if ($tapa == 'tuonti' or $tapa == 'tuontivienti') {

			if ($intrlasrow["maalahetys"] == '') {
				$virhetxt .= t($row["tapa"]).": <a href='tilauskasittely/vientitilauksen_lisatiedot.php?tapa=$tapa&tee=K&otunnus=$intrlasrow[tunnus]&lopetus=$lopetus_intra1$lopetus_intra2'>$intrlasrow[laskunro]</a>. ".t("L�hetysmaa puuttuu")."!<br>";
				$virhe++;
			}
			elseif ($intrlasrow["maalahetys"] == $yhtiorow["maa"]) {
				$virhetxt .= t($row["tapa"]).": <a href='tilauskasittely/vientitilauksen_lisatiedot.php?tapa=$tapa&tee=K&otunnus=$intrlasrow[tunnus]&lopetus=$lopetus_intra1$lopetus_intra2'>$intrlasrow[laskunro]</a>. ".t("L�hetysmaa ei voi olla sama kuin yhti�n kotimaa")."!<br>";
				$virhe++;
			}
			else {
				$query = "	SELECT distinct koodi, eu
							FROM maat
							WHERE koodi = '$intrlasrow[maalahetys]'";
				$maaresult = mysql_query($query) or pupe_error($query);

				if (mysql_num_rows($maaresult) == 0) {
					$virhetxt .= t($row["tapa"]).": <a href='tilauskasittely/vientitilauksen_lisatiedot.php?tapa=$tapa&tee=K&otunnus=$intrlasrow[tunnus]&lopetus=$lopetus_intra1$lopetus_intra2'>$intrlasrow[laskunro]</a>. ".t("L�hetysmaa on virheellinen")."!<br>";
					$virhe++;
				}
				else {
					$eurow = mysql_fetch_array($maaresult);
					if ($eurow["eu"] == "") {
						$virhetxt .= t($row["tapa"]).": <a href='tilauskasittely/vientitilauksen_lisatiedot.php?tapa=$tapa&tee=K&otunnus=$intrlasrow[tunnus]&lopetus=$lopetus_intra1$lopetus_intra2'>$intrlasrow[laskunro]</a>. ".t("L�hetysmaa on virheellinen").": NON-EU!<br>";
						$virhe++;
					}
				}
			}

		}

		// vientispecifig tarkistuksia
		if ($tapa == 'vienti' or $tapa == 'vientituonti') {

			if ($intrlasrow["maamaara"] == '') {
				$virhetxt .= t($row["tapa"]).": <a href='tilauskasittely/vientitilauksen_lisatiedot.php?tapa=$tapa&tee=K&otunnus=$intrlasrow[tunnus]&lopetus=$lopetus_intra1$lopetus_intra2'>$intrlasrow[laskunro]</a>. ".t("M��r�maa puuttuu")."!<br>";
				$virhe++;
			}
			elseif ($intrlasrow["maamaara"] == $yhtiorow["maa"]) {
				$virhetxt .= t($row["tapa"]).": <a href='tilauskasittely/vientitilauksen_lisatiedot.php?tapa=$tapa&tee=K&otunnus=$intrlasrow[tunnus]&lopetus=$lopetus_intra1$lopetus_intra2'>$intrlasrow[laskunro]</a>. ".t("M��r�maa ei voi olla sama kuin yhti�n kotimaa")."!<br>";
				$virhe++;
			}
			else {
				$query = "	SELECT distinct koodi, eu
							FROM maat
							WHERE koodi = '$intrlasrow[maamaara]'";
				$maaresult = mysql_query($query) or pupe_error($query);

				if (mysql_num_rows($maaresult) == 0) {
					$virhetxt .= t($row["tapa"]).": <a href='tilauskasittely/vientitilauksen_lisatiedot.php?tapa=$tapa&tee=K&otunnus=$intrlasrow[tunnus]&lopetus=$lopetus_intra1$lopetus_intra2'>$intrlasrow[laskunro]</a>. ".t("M��r�maa on virheellinen")."!<br>";
					$virhe++;
				}
				else {
					$eurow = mysql_fetch_array($maaresult);
					if ($eurow["eu"] == "") {
						$virhetxt .= t($row["tapa"]).": <a href='tilauskasittely/vientitilauksen_lisatiedot.php?tapa=$tapa&tee=K&otunnus=$intrlasrow[tunnus]&lopetus=$lopetus_intra1$lopetus_intra2'>$intrlasrow[laskunro]</a>. ".t("M��r�maa on virheellinen").": NON-EU!<br>";
						$virhe++;
					}
				}
			}

		}

		if ((int) $intrlasrow["kauppatapahtuman_luonne"] <= 0) {
		   	$virhetxt .= t($row["tapa"]).": <a href='tilauskasittely/vientitilauksen_lisatiedot.php?tapa=$tapa&tee=K&otunnus=$intrlasrow[tunnus]&lopetus=$lopetus_intra1$lopetus_intra2'>$intrlasrow[laskunro]</a>. ".t("Kauppatapahtuman luonne puuttuu")."!<br>";
			$virhe++;
		}

		if ((int) $intrlasrow["kuljetusmuoto"] == 0) {
		    $virhetxt .= t($row["tapa"]).": <a href='tilauskasittely/vientitilauksen_lisatiedot.php?tapa=$tapa&tee=K&otunnus=$intrlasrow[tunnus]&lopetus=$lopetus_intra1$lopetus_intra2'>$intrlasrow[laskunro]</a>. ".t("Kuljetusmuoto puuttuu")."!<br>";
			$virhe++;
		}

		if ($tapa == "tuontivienti") {
			$tapa = "tuonti";
		}

		if ($tapa == "vientituonti") {
			$tapa = "vienti";
		}

	}

	if ($virhetxt != "") {
		$virhetxt = $specutxt . $virhetxt;
	}

?>