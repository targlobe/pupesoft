<?php

require_once('tilauskasittely/luo_myyntitilausotsikko.inc');

$query = "  SELECT laskun_lisatiedot.sopimus_lisatietoja,
			laskun_lisatiedot.saate,
			lasku.olmapvm,
			lasku.clearing,
			tilausrivi.*,
			lasku.varasto,
			lasku.tilaustyyppi
			FROM laskun_lisatiedot
			JOIN tilausrivi
			ON ( tilausrivi.yhtio = laskun_lisatiedot.yhtio
				AND tilausrivi.otunnus = laskun_lisatiedot.otunnus )
			JOIN lasku
			ON ( lasku.yhtio = laskun_lisatiedot.yhtio
				AND lasku.tunnus = laskun_lisatiedot.otunnus)
			WHERE laskun_lisatiedot.yhtio = '{$kukarow['yhtio']}'
			AND laskun_lisatiedot.otunnus = '{$tilausnumero}'";
$result = pupe_query($query);

$tilausrivit = array();

while ($tilausrivi = mysql_fetch_assoc($result)) {
	$tilausrivit[] = $tilausrivi;
	//setataan muuttujat monta kertaa
	$tarjouksen_asiakkaat = $tilausrivi['sopimus_lisatietoja'];
	$clearing = $tilausrivi['clearing'];
	$alkuperainen_varasto = $tilausrivi['varasto'];
	$viimeinen_voimassaolo_pvm = $tilausrivi['olmapvm'];
	$saate_teksti = $tilausrivi['saate'];
	$tilaustyyppi = $tilausrivi['tilaustyyppi'];
}

$tarjouksen_asiakkaat = explode(',', $tarjouksen_asiakkaat);

$dummy_tarjouksen_tilausnumero = $tilausnumero;

foreach ($tarjouksen_asiakkaat as $tarjous_asiakas) {

	$kukarow['kesken'] = 0;

	if ($toim == "EXTTARJOUS") {
		$tilausnumero = luo_myyntitilausotsikko('TARJOUS', $tarjous_asiakas, '', '', '', '', '', '', $clearing);
	}
	else {
		$tilausnumero = luo_myyntitilausotsikko('ENNAKKO', $tarjous_asiakas, '', '', '', '', '', '', $clearing);
	}

	//Halutaan pit�� asiakkaille kopioitavissa tarjouksissa myyj�n valitsema varasto
	$query = "	UPDATE lasku
				JOIN laskun_lisatiedot ON (laskun_lisatiedot.yhtio = lasku.yhtio AND laskun_lisatiedot.otunnus = lasku.tunnus)
				SET lasku.varasto = '{$alkuperainen_varasto}',
				lasku.olmapvm = '{$viimeinen_voimassaolo_pvm}',
				laskun_lisatiedot.saate = '{$saate_teksti}',
				lasku.clearing = '{$clearing}',
				lasku.tilaustyyppi = '{$tilaustyyppi}'
				WHERE lasku.yhtio = '{$kukarow['yhtio']}'
				AND lasku.tunnus = '{$tilausnumero}'";
	pupe_query($query);

	$query = "  SELECT *
				FROM lasku
				JOIN laskun_lisatiedot
				ON ( laskun_lisatiedot.yhtio = lasku.yhtio
					AND laskun_lisatiedot.otunnus = lasku.tunnus )
				WHERE lasku.yhtio = '{$kukarow['yhtio']}'
				AND lasku.tunnus = '{$tilausnumero}'";
	$result = pupe_query($query);

	$laskurow = mysql_fetch_assoc($result);

	$kukarow['kesken'] = $tilausnumero;

	foreach ($tilausrivit as $tilausrivi) {

		if ($tilausrivi['tunnus'] == $tilausrivi['perheid'] or $tilausrivi['perheid'] == 0) {

			// Ext-ennakolle tulee hinnat aina nettona, lasketaan alennukset pois
			if ($toim == "EXTENNAKKO") {
				$tilausrivi['hinta'] = $tilausrivi['hinta'] * (1 - $tilausrivi['ale1'] / 100) * (1 - $tilausrivi['ale2'] / 100) * (1 - $tilausrivi['ale3'] / 100);
				$tilausrivi['ale1'] = 0;
				$tilausrivi['ale2'] = 0;
				$tilausrivi['ale3'] = 0;
				$tilausrivi['netto'] = 'N';
			}

			$trow = hae_tuote($tilausrivi['tuoteno']);

			$parametrit = array(
				'trow'				 => $trow,
				'tuoteno'			 => $trow['tuoteno'],
				'laskurow'			 => $laskurow,
				'kpl'				 => $tilausrivi['varattu'],
				'ale1'				 => $tilausrivi['ale1'],
				'ale2'				 => $tilausrivi['ale2'],
				'ale3'				 => $tilausrivi['ale3'],
				'netto'				 => $tilausrivi['netto'],
				'hinta'				 => $tilausrivi['hinta'],
				'varataan_saldoa'	 => 'EI',
			);
			
			lisaa_rivi($parametrit);
		}
	}

	require_once('tilaus-valmis.inc');
}

$query = "  UPDATE lasku
			SET tila = 'D'
			WHERE yhtio = '{$kukarow['yhtio']}'
			AND tunnus = '{$dummy_tarjouksen_tilausnumero}'";
pupe_query($query);
