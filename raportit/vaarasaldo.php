<?php

	//* T�m� skripti k�ytt�� slave-tietokantapalvelinta *//
	$useslave = 1;

	require('../inc/parametrit.inc');

	if (!isset($kka)) $kka = date("m",mktime(0, 0, 0, date("m"), date("d")-1, date("Y")));
	if (!isset($vva)) $vva = date("Y",mktime(0, 0, 0, date("m"), date("d")-1, date("Y")));
	if (!isset($ppa)) $ppa = date("d",mktime(0, 0, 0, date("m"), date("d")-1, date("Y")));

	if (!isset($kkl)) $kkl = date("m");
	if (!isset($vvl)) $vvl = date("Y");
	if (!isset($ppl)) $ppl = date("d");

	if (!isset($nayta)) $nayta = '';
	if (!isset($tee)) $tee = '';

	echo "<font class='head'>",t("Ker�yspoikkeamat"),":</font><hr>";

	if ($tee != '') {

		$aikalisa = $rivien_aika == 'laskutettuaika' ? "laskutettuaika" : "kerattyaika";

		$query = "	SELECT lasku.nimi asiakas, tilausrivi.tuoteno, tilausrivi.nimitys, tilausrivi.tilkpl, tilausrivi.kpl, tilausrivi.keratty,
					concat_ws(' ',tilausrivi.hyllyalue, tilausrivi.hyllynro, tilausrivi.hyllyvali, tilausrivi.hyllytaso) tuotepaikka,
					tilausrivi.nimitys, tilausrivi.yksikko, tilausrivi.hyllyalue, tilausrivi.hyllynro, tilausrivi.hyllytaso, tilausrivi.hyllyvali,
					concat(lpad(upper(tilausrivi.hyllyalue), 5, '0'),lpad(upper(tilausrivi.hyllynro), 5, '0'),lpad(upper(tilausrivi.hyllyvali), 5, '0'),lpad(upper(tilausrivi.hyllytaso), 5, '0')) sorttauskentta
					FROM tilausrivi
					JOIN lasku ON (tilausrivi.yhtio=lasku.yhtio and tilausrivi.otunnus=lasku.tunnus)
					WHERE tilausrivi.yhtio = '{$kukarow['yhtio']}'
					and tilausrivi.tyyppi  = 'L'
					and tilausrivi.{$aikalisa} >= '{$vva}-{$kka}-{$ppa}'
					and tilausrivi.{$aikalisa} <= '{$vvl}-{$kkl}-{$ppl}'
					and tilausrivi.var not in ('P','J')
					and tilausrivi.tilkpl <> tilausrivi.kpl
					ORDER BY sorttauskentta, tuoteno";
		$result = mysql_query($query) or pupe_error($query);

		if (mysql_num_rows($result) > 0 ) {
			echo "<table><tr>
					<th>".t("Varastopaikka")."</th>
					<th>".t("Tuoteno")."</th>
					<th>".t("Nimitys")."</th>
					<th>".t("Asiakas")."</th>
					<th>".t("Tilattu")."</th>
					<th>".t("Toimitettu")."</th>
					<th>".t("Tilauksessa")."</th>
					<th>".t("Ensimm�inen toimitus")."</th>
					<th>".t("Hyllyss�")."</th>
					<th>".t("Saldo")."</th>
					<th>".t("Ker��j�")."</th></tr>";

			while ($row = mysql_fetch_array($result)) {

				if ($nayta == 'ei_ylijaamia' and $row['tilkpl'] < $row['kpl']) continue;

				list($saldo, $hyllyssa, $myytavissa) = saldo_myytavissa($row["tuoteno"]);

				//saldolaskentaa tulevaisuuteen
				$query = "	SELECT sum(varattu) varattu,
							min(toimaika) toimaika
							FROM tilausrivi
							WHERE yhtio = '$kukarow[yhtio]'
							and tuoteno = '$row[tuoteno]'
							and tyyppi='O'
							and varattu > 0";
				$result2 = mysql_query($query) or pupe_error($query);
				$prow = mysql_fetch_array($result2);

				echo "<tr>
						<td>$row[tuotepaikka]</td>
						<td>$row[tuoteno]</td>
						<td>".t_tuotteen_avainsanat($row, 'nimitys')."</td>
						<td>$row[asiakas]</td>
						<td align='right'>{$row['tilkpl']}</td>
						<td align='right'>{$row['kpl']}</td>
						<td align='right'>{$prow['varattu']}</td>
						<td>$prow[toimaika]</td>
						<td align='right'>{$hyllyssa}</td>
						<td align='right'>{$saldo}</td>
						<td>$row[keratty]</td></tr>";
			}

			echo "</table>";

		}
		else {
			echo "<br>".t("Ei tuotteita v��rill� saldoilla")."!<br>";
		}
	}

	//K�ytt�liittym�
	echo "<br>";
	echo "<table><form method='post'>";
	echo "<input type='hidden' name='tee' value='kaikki'>";

	echo "<tr>";
	echo "<th>",t("N�yt�"),"</th>";
	echo "<td><select name='nayta'>";
	echo "<option value=''>",t("Kaikki poikkeamat"),"</option>";

	$sel = $nayta == 'ei_ylijaamia' ? ' selected' : '';

	echo "<option value='ei_ylijaamia'{$sel}>",t("Kaikki paitsi rivit jossa ker��j� on ker�nnyt tilattua enemm�n"),"</option>";
	echo "</select></td>";
	echo "</tr>";

	echo "<tr>";
	echo "<th>",t("Hae rivit ajan mukaan"),"</th>";
	echo "<td><select name='rivien_aika'>";
	echo "<option value=''>",t("Ker�ttyaika"),"</option>";

	$sel = $rivien_aika == 'laskutettuaika' ? ' selected' : '';

	echo "<option value='laskutettuaika'{$sel}>",t("Laskutettuaika"),"</option>";
	echo "</select></td>";
	echo "</tr>";

	echo "<tr><th>",t("Sy�t� alkup�iv�m��r� (pp-kk-vvvv)"),"</th>
			<td><input type='text' name='ppa' value='{$ppa}' size='3'>
			<input type='text' name='kka' value='{$kka}' size='3'>
			<input type='text' name='vva' value='{$vva}' size='5'></td>
			</tr><tr><th>",t("Sy�t� loppup�iv�m��r� (pp-kk-vvvv)"),"</th>
			<td><input type='text' name='ppl' value='{$ppl}' size='3'>
			<input type='text' name='kkl' value='{$kkl}' size='3'>
			<input type='text' name='vvl' value='{$vvl}' size='5'>";
	echo "<td class='back'><input type='submit' value='",t("Aja raportti"),"'></td></tr></table>";

	require ("inc/footer.inc");
