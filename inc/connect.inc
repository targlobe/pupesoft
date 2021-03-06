<?php

require("salasanat.php");

// $masterlink on aina linkki master-kantaan.
// $link on linkki master- tai slave-kantaan. Jos slave on k�yt�ss� ja aktivoitu $useslave-muuttujalla niin silloin tuo pointaa siis slave-kantaan.
// $temporarylink on linkki omaan kantaan johon pupe tekee temporary tablet. Jos $tempdbhost:ia ei oo m��ritelty, niin silloin $temporarylink pointaa master-kantaan.

if (isset($useslave)) {
	$useslave = (int) $useslave;
}
else {
	$useslave = 0;
}

// tarkistetaan onko annettu slave oikea
if ($useslave > 0) {
	// katsotaan onko haluttu useslave setattu, jos ei ole tai se on tyhj�� laitetaan ykk�nen
	if (!isset($slavedb[$useslave])) {
		$useslave = 1;
	}
	elseif ($slavedb[$useslave] == '') {
		$useslave = 1;
	}

	// katsotaan onko haluttu useslave setattu, jos ei ole tai se on tyhj�� laitetaan nolla eli masteri
	if (!isset($slavedb[$useslave])) {
		$useslave = 0;
	}
	elseif ($slavedb[$useslave] == '') {
		$useslave = 0;
	}
}

// Jos Pupeen on konffattu oma database temporary tableja varten
if (isset($tempdbhost) and $tempdbhost != "") {
	$temporarylink = mysql_connect($tempdbhost, $tempdbuser, $tempdbpass) or die ("Ongelma tietokantapalvelimessa $tempdbhost");
	mysql_select_db($tempdbkanta, $temporarylink) or die ("Tietokantaa $tempdbkanta ei l�ydy palvelimelta $tempdbhost! (connect.inc)");
	mysql_set_charset("latin1", $temporarylink);
}

// K�ytet��n slave kantaa
if ($useslave > 0) {
	// Otetaan yhteys my�s master-kantaan koska tietyiss� paikoissa t�t� tarvitaan
	$masterlink = mysql_connect($dbhost, $dbuser, $dbpass) or die ("Ongelma tietokantapalvelimessa $dbhost");
	mysql_select_db($dbkanta, $masterlink) or die ("Tietokantaa $dbkanta ei l�ydy palvelimelta $dbhost! (connect.inc)");
	mysql_set_charset("latin1", $masterlink);
	mysql_query("set group_concat_max_len=1000000", $masterlink);

	$link = mysql_connect($slavedb[$useslave], $slaveuser[$useslave], $slavepass[$useslave]) or die ("Ongelma tietokantapalvelimessa: '$slavedb[$useslave]'");
}
else {
	$link = mysql_connect($dbhost, $dbuser, $dbpass) or die ("Ongelma tietokantapalvelimessa $dbhost");
	$masterlink = $link;
}

mysql_select_db($dbkanta, $link) or die ("Tietokantaa $dbkanta ei l�ydy palvelimelta $dbhost! (connect.inc)");
mysql_set_charset("latin1", $link);
mysql_query("set group_concat_max_len=1000000", $link);

if (!isset($temporarylink)) $temporarylink = $masterlink;
