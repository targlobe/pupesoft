<?php

	$otsikko 		= 'Yhteyshenkil�t';
	$otsikko_nappi 	= 'yhteyshenkil�';

	if ($from == "") {
		// HUOM HUOM: eth�n muuta n�it� ilman, ett� korjaat my�s yllapito.php:ss� iframen lukitse_avaimeen muuttujat
		$kentat = "tunnus, if(tyyppi='T', (SELECT nimi from toimi where yhteyshenkilo.yhtio=toimi.yhtio and yhteyshenkilo.liitostunnus=toimi.tunnus and yhteyshenkilo.tyyppi='T'), (SELECT nimi from asiakas where yhteyshenkilo.yhtio=asiakas.yhtio and yhteyshenkilo.liitostunnus=asiakas.tunnus and yhteyshenkilo.tyyppi='A')) 'asiakas/toimittaja', nimi, titteli, concat_ws('<br>', puh, gsm, fax) 'puh/gsm/fax', email, www";
		$hakukentat = "tunnus, liitostunnus, nimi, titteli, puh/gsm/fax, email, www";
	}
	else {
		// HUOM HUOM: eth�n muuta n�it� ilman, ett� korjaat my�s yllapito.php:ss� iframen lukitse_avaimeen muuttujat
		$kentat = "tunnus, nimi, titteli, concat_ws('<br>', puh, gsm, fax) 'puh/gsm/fax', email, www, liitostunnus HIDDEN_liitostunnus";
		$hakukentat = "tunnus, nimi, titteli, puh/gsm/fax, email, www, liitostunnus";
	}

	if ($laji == "T") {
		$rajauslisa = " and yhteyshenkilo.tyyppi='T' ";
	}
	else {
		$rajauslisa = " and yhteyshenkilo.tyyppi='A' ";
	}

	$jarjestys = 'nimi';
?>