<?php

if (@include_once("inc/functions.inc"));
elseif (@include_once("functions.inc"));
else exit;

// Parametrit:
# $ftphost --> FTP-palvelin
# $ftpuser --> K�ytt�j�tunnus
# $ftppass --> Salasana
# $ftppath --> Kansio FTP-palvelimella jonne failit ty�nnet��n
# $ftpfile --> Siirrett�v� tiedosto
# $ftpport --> Custom portti, ei pakollinen
# $ftpfail --> Kansio jonne laitetaan failit jos siirto feilaa, ei pakollinen
# $ftpsucc --> Kansio jonne tallennetaan failit jos siirto onnistuu, ei pakollinen
# $ftptype --> FTP-palvelimen tyyppi esim "AS400", ei pakollinen
# $ftptmpr --> K�ytet��nk� .TMP p��tett� siirron aikana.

// Palautetaan:
# $palautus
# $syy

// Debug:
# echo "ftphost --> $ftphost<br>";
# echo "ftpuser --> $ftpuser<br>";
# echo "ftppass --> $ftppass<br>";
# echo "ftppath --> $ftppath<br>";
# echo "ftpfile --> $ftpfile<br>";
# echo "ftpport --> $ftpport<br>";
# echo "ftpfail --> $ftpfail<br>";
# echo "ftpsucc --> $ftpsucc<br>";

$syy			 = "";
$palautus		 = 0;
$tulos_ulos_ftp	 = "";
$ncftplisa		 = "";
$renameftpfile	 = isset($renameftpfile) ? basename(trim($renameftpfile)) : "";
$palautus_pyynto = isset($palautus_pyynto) ? trim($palautus_pyynto) : "";
$filenimi		 = basename($ftpfile);
$ftptmpr		 = !isset($ftptmpr) ? TRUE : $ftptmpr;

if ($ftphost == 'localhost' and $ftppath != '' and is_dir($ftppath) and is_writable($ftppath) and $ftpfile != '' and is_file($ftpfile) and is_writable($ftpfile)) {
	
	$ftppath = rtrim(realpath($ftppath), "/")."/";
	$file_from = realpath($ftpfile);
	$file_to = $ftppath.$filenimi;

	if (rename($file_from, $file_to)) {
		$tulos_ulos_ftp .= t("Tiedoston siirto onnistui")."<br>\n";
	}
	else {
		$tulos_ulos_ftp .= t("Tiedoston siirto ep�onnistui")."<br>\n";
	}
}
elseif ($ftphost == '' or $ftpuser == '' or $ftppass == '' or $ftppath == '' or $ftpfile == '') {
	$tulos_ulos_ftp .= "<font class='error'>".t("L�hetykseen tarvittavia tietoja puuttuu")."! (host, user, pass, path, file)</font><br>\n";
	$palautus = 9;
	$syy = t("L�hetykseen tarvittavia tietoja puuttuu");
}
else {
	//l�hetet��n tiedosto

	// Onko k�yt�ss� custom portti
	if (isset($ftpport) and (int) $ftpport > 0) {
		$ftpport = (int) $ftpport;

		$ncftplisa = "-P $ftpport";

		$conn_id = ftp_connect($ftphost, $ftpport);
	}
	else {
		$conn_id = ftp_connect($ftphost);
	}

	// jos connectio ok, kokeillaan loginata
	if ($conn_id) {
		$login_result = ftp_login($conn_id, $ftpuser, $ftppass);
	}

	// jos viimeinen merkki pathiss� ei ole kauttaviiva lis�t��n kauttaviiva...
	if (substr($ftppath, -1) != "/") {
		$ftppath .= "/";
	}

	if ($renameftpfile != "") {
		// ncftp:ll� ei hanskata uudelleennime�mist�, joten laitetaan suoraan lopullisella nimell�...
		$cmd = "/usr/bin/ncftpput -u ".escapeshellarg($ftpuser)." -p ".escapeshellarg($ftppass)." $ncftplisa ".escapeshellarg($ftphost)." ".escapeshellarg($ftppath)." ".escapeshellarg($renameftpfile);
	}
	else {
		$cmd = "/usr/bin/ncftpput -u ".escapeshellarg($ftpuser)." -p ".escapeshellarg($ftppass)." $ncftplisa ".escapeshellarg($ftphost)." ".escapeshellarg($ftppath)." ".escapeshellarg($ftpfile);
	}

	// jos login ok kokeillaan uploadata
	if (isset($login_result) and $login_result) {

		// Tehd��n NAMEFMT komento, jos kyseess� on iSeries/AS400 palvelin
		if (isset($ftptype) and $ftptype == "AS400") {
			$quote = ftp_site($conn_id, "NAMEFMT 1");
		}

		if ($ftptmpr) {
			// K�ytet��n aina .TMP p��tett� kun siirto on kesken
			$ftpfile_tmp = $filenimi.".TMP";
		}
		else {
			// Ei k�ytet� .TMP p��tett�
			$ftpfile_tmp = $filenimi;
		}

		// Kokeillaan passiivista siirtoa
		ftp_pasv($conn_id, TRUE);
		$upload = @ftp_put($conn_id, $ftppath.$ftpfile_tmp, realpath($ftpfile), FTP_ASCII);

		// Kokeillaan aktiivista siirtoa jos passiivi feilaa
		if ($upload === FALSE) {
			ftp_pasv($conn_id, FALSE);
			$upload = ftp_put($conn_id, $ftppath.$ftpfile_tmp, realpath($ftpfile), FTP_ASCII);
		}

		if ($ftptmpr) {
			// Otetaan .TMP p��te veks ku siirto on valmis
			if (isset($ftpport) and (int) $ftpport > 0 and $upload === TRUE) {
				// Jos $ftpport on k�yt�ss� niin k�ytet��n n�it� komentoja.
				// N�ytt�� silt�, ett� ftp_rename ei osaa hanskata renametusta kun k�ytet��n custom porttia
				$rename = ftp_raw($conn_id, "RNFR {$ftppath}{$ftpfile_tmp}");
	            $rename = ftp_raw($conn_id, "RNTO {$ftppath}{$filenimi}");

				if (stripos($rename[0], "command successful") === FALSE and stripos($rename[0], "file renamed") === FALSE) {
					$rename1 = FALSE;
				}
			}
			elseif ($upload === TRUE) {
				$rename1 = ftp_rename($conn_id, "{$ftppath}{$ftpfile_tmp}", "{$ftppath}{$filenimi}");

				// Jos rename ep�onnistuu, koitetaan poistaa .TMP tiedosto
				if ($rename1 === FALSE) {
					$delete1 = ftp_delete($conn_id, "{$ftppath}{$ftpfile_tmp}");

					if ($delete1 === FALSE) {
						$raw = ftp_raw($conn_id, "quote dele {$ftppath}{$ftpfile_tmp}");
					}
				}
			}
		}

		// Pit��k� faili viel� nimet� kokonaan uudestaan (Finvoice iPost)
		if ($upload === TRUE and $renameftpfile != "") {
			$rename2 = ftp_rename($conn_id, "{$ftppath}{$filenimi}", "{$ftppath}{$renameftpfile}");

			// Jos rename ep�onnistuu, koitetaan poistaa turha v�litiedosto
			if ($rename2 === FALSE) {
				$rename2 = ftp_delete($conn_id, "{$ftppath}{$filenimi}");
				if ($rename2 === FALSE) {
					$raw = ftp_raw($conn_id, "quote dele {$ftppath}{$filenimi}");
				}
				$rename2 = FALSE;
			}
		}

		// Kokeillaan ncftp-ohjelmalla, uudelleenime�minen ei onnaa joten laitetaan suoraan lopullisella nimell�
		if ($upload === FALSE) {
			exec($cmd, $skit, $upload_ncftp);

			if ($upload_ncftp == 0) {
				$upload = TRUE;
			}
		}
	}

	if ($conn_id) {
		ftp_close($conn_id);
	}

	// mik� feilas?
	if (isset($conn_id) and $conn_id === FALSE) {
		$palautus = 1;
	}
	if (isset($login_result) and $login_result === FALSE) {
		$palautus = 2;
	}
	if (isset($upload) and $upload === FALSE) {
		$palautus = 3;
	}
	if (isset($rename1) and $rename1 === FALSE) {
		$palautus = 4;
	}
	if (isset($rename2) and $rename2 === FALSE) {
		$palautus = 5;
	}
	if (isset($delete1) and $delete1 === FALSE) {
		$palautus = 6;
	}

	// Siirto onnistui
	if ($palautus == 0 and isset($ftpsucc) and $ftpsucc != "") {
		// Siirret��n faili talteen jos siirto onnasi ja ok failit halutaan tallentaa
		rename($ftpfile, $ftpsucc."/".$filenimi);
	}
	elseif ($palautus != 0 and isset($ftpfail) and $ftpfail != "") {
		// Siirret��n faili outboxiin odottamaan uutta yrityst� jos siirto feilasi
		rename($ftpfile, $ftpfail."/".$filenimi);
	}

	// jos siirto ep�onnistuu ja faildirri ei oo setattu nin l�hetet��n maili
	// jos faildirri ON setattu niin ei ekoteta eik� meilata mitt��n
	if ($palautus != 0 and (!isset($ftpfail) or $ftpfail == "")) {
		// ncftpput:in exit valuet
		switch ($palautus) {
			case  1:
				$syy = "Could not connect to remote host. ($ftphost)";
				break;
			case  2:
				$syy = "Could not login to remote host ($ftpuser, $ftppass)";
				break;
			case  3:
				$syy = "Transfer failed ($ftppath, ".realpath($ftpfile).")";
				break;
			case  4:
				$syy = "Rename failed ($ftppath, {$ftppath}{$ftpfile_tmp} --> {$ftppath}{$filenimi})";
				break;
			case  5:
				$syy = "Rename failed ($ftppath, {$ftppath}{$filenimi} --> {$ftppath}{$renameftpfile})";
				break;
			case  6:
				$syy = "Delete failed ({$ftppath}{$ftpfile_tmp})";
				break;
			default:
				$syy = t("Tuntematon errorkoodi")." ($palautus)!!";
		}

		$rivi  = "{$_SERVER['SCRIPT_NAME']}\n";
		$rivi .= "\n";
		$rivi .= t("Tiedoston")." '$ftpfile' ".t("l�hetys ep�onnistui")."!\n";
		$rivi .= "\n";
		$rivi .= "$cmd\n";
		$rivi .= "\n";
		$rivi .= "$syy\n";

		$boob = mail($yhtiorow['alert_email'], mb_encode_mimeheader(t("Tiedostonsiirto ep�onnistui")."!", "ISO-8859-1", "Q"), $rivi, "From: ".mb_encode_mimeheader($yhtiorow["nimi"], "ISO-8859-1", "Q")." <$yhtiorow[postittaja_email]>\n", "-f $yhtiorow[postittaja_email]");

		if ($boob === FALSE) {
			$tulos_ulos_ftp .= "<font class='error'>\n";
			$tulos_ulos_ftp .= t("Tiedostonsiirto ep�onnistui! Ja meilin l�hett�minen")." $yhtiorow[alert_email] ".t("ep�onnistui my�s!")."\n";
			$tulos_ulos_ftp .= "</font><br>\n";

			$tulos_ulos_ftp .= "<br><pre>$rivi</pre><br>\n";
		}
		else {
			$tulos_ulos_ftp .= "<font class='error'>FAILED! (".t("l�hetettiin s�hk�posti")." $yhtiorow[alert_email])</font><br>\n";
		}
	}
	elseif ($palautus_pyynto != "") {
		$rivi  = "{$_SERVER['SCRIPT_NAME']}\n";
		$rivi .= "\n";
		$rivi .= t("Tiedoston")." '$ftpfile' ".t("l�hetys onnistui")."!\n";
		$rivi .= "\n";
		$rivi .= "$ftpuser @ $ftphost : $ftppath\n";
		$rivi .= "\n";
		$rivi .= "\n";

		$silari = mail($yhtiorow['alert_email'], mb_encode_mimeheader(t("Tiedostonsiirto onnistui")."!", "ISO-8859-1", "Q"), $rivi, "From: ".mb_encode_mimeheader($yhtiorow["nimi"], "ISO-8859-1", "Q")." <$yhtiorow[postittaja_email]>\n", "-f $yhtiorow[postittaja_email]");
	}
}

//echotaan rudulle jos kyseess� ei ole batch-ajo
if (!isset($tulos_ulos) or $tulos_ulos == "") {
	echo $tulos_ulos_ftp;
}
elseif ($tulos_ulos == "edi") {
	$edi_ulos .= strip_tags($tulos_ulos_ftp);
}
