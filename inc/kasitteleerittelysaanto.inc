<?php

	$query = "	SELECT tilino
				FROM tiliotesaanto
				WHERE yhtio 	= '$yritirow[yhtio]'
				and erittely	= 'o'
				and pankkitili 	= '$pankkitilino'
				and koodi 		= '$koodi'
				and koodiselite = '$koodiselite'";
	$tiliotesaantoresult = mysql_query($query) or pupe_error($query);

	if (mysql_num_rows($tiliotesaantoresult) > 0) { // Jotain l�ytyy
		$kuittikoodi=' ';
		echo "<font class='message'>".t("Erittely pyydettiin k�sittelem��n")."</font><br>\n";
	}

?>
