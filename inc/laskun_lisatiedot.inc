<?php
	$otsikko = 'Laskun lis�tiedot';
	$otsikko_nappi = 'laskun lis�tiedot';

	$kentat = 'tunnus, otunnus, laskutus_nimi, laskutus_nimitark, laskutus_osoite';
	$jarjestys = 'tunnus DESC';

	$uusilukko = "LUKOSSA";

	if (!isset($lukitse_avaimeen) or (int) $lukitse_avaimeen == 0) $rajauslisa = " and yhtio='�L�N�YT�' ";
?>