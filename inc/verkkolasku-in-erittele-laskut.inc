<?php
	function erittele_laskut($file) {

		global $laskut, $origlaskut;

		$luotiinlaskuja = 0;
		$type = "";
		$boob = "";
		$soap = "";
		$ebid = "";

		$poistettavat = array('/','.',':'); //Merkit, jotka poistetaan filenimest�

		if (stripos(file_get_contents($file), "Finvoice") !== FALSE) {
			foreach (file($file) as $ln) {

				$ln = trim($ln); // varmistaan ett� ei j�� roskia

				//Haetaan Finvoicen tunnusmerkkej�
				if (preg_match("/\<SOAP\-ENV\:Envelope.*\>/i", $ln) > 0) {
					$type = "SOAP";
				}

				if (preg_match("/\<Finvoice.*?Version\=\"([1-2]\.[0-3]{1,2})\".*?\>/i", $ln, $lnversion) > 0) {
					$type 			 = "FINVOICE";
					$finvoiceversion = (float) $lnversion[1];

					// 	lis�t��n varmasti oikeat DOCTYPE m��ritys
					//	T�m� siksi ett� haluamme k�ytt�� paikallista kuvausta eik� ladata kuvausta verkosta!

					if ($finvoiceversion >= 2) {
						$ln = "<?xml version=\"1.0\" encoding=\"ISO-8859-15\"?>\n<?xml-stylesheet type=\"text/xsl\" href=\"datain/Finvoice_2.0.xsl\"?>\n<Finvoice Version=\"$lnversion[1]\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"datain/Finvoice_2.0.xsd\">";
					}
					elseif ($finvoiceversion == 1.3) {
						$ln = "<?xml version=\"1.0\" encoding=\"ISO-8859-15\"?>\n<!DOCTYPE Finvoice SYSTEM \"datain/Finvoice_1.3.dtd\">\n<?xml-stylesheet type=\"text/xsl\" href=\"datain/Finvoice_1.3.xsl\"?>\n".$ln;
					}
					else {
						$ln = "<?xml version=\"1.0\" encoding=\"ISO-8859-15\"?>\n<!DOCTYPE Finvoice SYSTEM \"datain/Finvoice.dtd\">\n<?xml-stylesheet type=\"text/xsl\" href=\"datain/Finvoice.xsl\"?>\n".$ln;
					}
				}

				//	Meill� oli SOAP kehys, t��lt� me saadaan EBID sender-messageid
				if (isset($type) and $type == "SOAP" and preg_match("/\<eb\:MessageId\>(.*?)\<\/eb\:MessageId\>/i", $ln, $messageid) > 0) {
					$ebid = $messageid[1];
					$ebid = str_replace($poistettavat,"_",$ebid);
				}

				if ($type == "SOAP") $soap .= $ln; // T�ss� soap kehys
				if ($type == "FINVOICE") $boob .= $ln."\n"; // Rakennetaan varsinainen finvoice tiedosto

				// 	Tiedostosta l�ytyi finvoiceaineisto
				if ($type == "FINVOICE" and stripos($ln, "</Finvoice>") !== FALSE) {

					// Oteaan SOAP-kehyksest� Receiver PartyId koska se ei nyt aina tulekaan niin kauniisti Finvoicessa, esim jos toimittajalla on huono asiakasrekisteri
					if ($soap != "" and preg_match("/\<eb\:To\>\<eb\:PartyId\>(.*?)\<\/eb\:PartyId\>\<eb\:Role\>Receiver\<\/eb\:Role\>\<\/eb\:To\>/i", $soap, $receiverparty) > 0) {

						// Jos tieto l�ytyy
						if (trim($receiverparty[1]) != "") {
							// L�ytyyk� BuyerOrganisationUnitNumber-segmentti
							if (preg_match("/\<BuyerOrganisationUnitNumber\>.*?\<\/BuyerOrganisationUnitNumber\>/i", $boob, $buyermatch)) {
								// Jos l�ytyy niin muokataan
								$boob = str_replace($buyermatch[0], "<BuyerOrganisationUnitNumber>".$receiverparty[1]."</BuyerOrganisationUnitNumber>", $boob);
							}
							else {
								// Muuten lis�t��n
								$boob = str_replace("</BuyerPartyDetails>", "</BuyerPartyDetails>\n<BuyerOrganisationUnitNumber>".$receiverparty[1]."</BuyerOrganisationUnitNumber>", $boob);
							}
						}
					}

					if ($ebid == "") {
						$ebid = "gen".substr(md5(rand()),0,20);
					}

					$path_parts = pathinfo($file);
					$filenimi = "finvoice-{$path_parts["filename"]}-$ebid.xml";
					$save = file_put_contents($laskut."/".$filenimi, $boob);

					if ($save !== FALSE) {
						$luotiinlaskuja++;
					}
					else {
						echo "Laskutiedoston tallennuksessa tapahtui virhe!\n";
					}

					// Haetaan taas uutta aineistoa
					$type = "";
					$boob = "";
					$soap = "";
					$ebid = "";
				}
			}
		}

		$xml = @simplexml_load_file($file);

		if ($xml !== FALSE) {

			$i = 1;

			// Unikon laskuaineisto
			if (isset($xml->LedgerJournalTable->LedgerJournalName) and $xml->LedgerJournalTable->LedgerJournalName == "ORAO") {

				$vakio_alku = '<?xml version="1.0"?><LedgerJournal><LedgerJournalTable><LedgerJournalName>ORAO</LedgerJournalName><Name>Ostolaskut Unikosta</Name>';
				$vakio_loppu = '</LedgerJournalTable></LedgerJournal>';

				// Lis�t��n ekaan tiedostoon alkutagit
				$xmlnimi = "ORAO-$i-".md5(uniqid(mt_rand(), true)).".xml";
				file_put_contents($laskut."/".$xmlnimi, $vakio_alku);

				foreach ($xml->LedgerJournalTable->LedgerJournalTrans as $lasku) {

					if ((string) $lasku->AccountType == "Vend" and $i > 1) {
						// Kun toimittaja vaihtuu, kirjoitetaan lopputagit...
						file_put_contents($laskut."/".$xmlnimi, $vakio_loppu, FILE_APPEND);

						// ...ja keksit��n uusi tiedostonimi...
						$xmlnimi = "ORAO-$i-".md5(uniqid(mt_rand(), true)).".xml";

						// ...ja kirjotetaan sinne alkutagit
						file_put_contents($laskut."/".$xmlnimi, $vakio_alku.$lasku->asXML());
					}
					else {
						file_put_contents($laskut."/".$xmlnimi, $lasku->asXML(), FILE_APPEND);
					}

					$i++;
				}

				// Lis�t��n viimeiseen tiedostoon lopputagit
				file_put_contents($laskut."/".$xmlnimi, $vakio_loppu, FILE_APPEND);
				$luotiinlaskuja++;
			}

			// Teccom laskuaineisto
			if (isset($xml->Document->attributes()->Type) and $xml->Document->attributes()->Type == "Invoice") {

				// Laitetaan teccomifile ensin talteen sellaisenaan
				$path_parts = pathinfo($file);
				$filenimi = "teccominvoice_{$path_parts['basename']}";
				copy($file, $origlaskut."/".$filenimi);

				$xmldata = file_get_contents($file);

				// poistetaan nonprintable merkit
				$xmldata = preg_replace("/[^\x20-\xFF\n]/", "", $xmldata);

				//	T�m� siksi ett� haluamme k�ytt�� paikallista kuvausta eik� ladata kuvausta verkosta!
				$xmldata = preg_replace("/^.*?<Invoice>/ms", "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<?xml-stylesheet type=\"text/xsl\" href=\"datain/teccom-invoice.xsl\"?>\n<Invoice>", $xmldata, 1);

				// Jos data ei oo UTF-8 muodossa niin laitetaan se siihen
				if (mb_detect_encoding($xmldata, 'UTF-8', true) === FALSE) {
        			$xmldata = utf8_encode($xmldata);
        		}

				$save = file_put_contents($file, $xmldata);
				$path_parts = pathinfo($file);

				rename($file, "{$laskut}/teccominvoice_{$path_parts['basename']}");
			}
		}

		return $luotiinlaskuja;
	}
