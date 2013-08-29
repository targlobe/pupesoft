$(document).ready(function() {
	bind_valitut_rivit_checkbox_click();
	bind_tarkista_tehtaan_saldot_click();
});

function bind_valitut_rivit_checkbox_click() {
	$('.valitut_rivit').on('click', function() {

		if ($(this).is(':checked')) {
			if ($(this).val().indexOf(',') > -1) {
				tunnukset_array = $(this).val().split(',');
				for (var i in tunnukset_array) {
					$('#jyvita_valitut_form').append("<input type='hidden' name='valitut_tilausrivi_tunnukset[]' class='tilausrivi_tunnukset' value='" + tunnukset_array[i] + "'>");
				}
			}
			else {
				$('#jyvita_valitut_form').append("<input type='hidden' name='valitut_tilausrivi_tunnukset[]' class='tilausrivi_tunnukset' value='" + $(this).val() + "'>");
			}
		}
		else {
			if ($(this).val().indexOf(',') > -1) {
				tunnukset_array = $(this).val().split(',');
				for (var x in tunnukset_array) {
					$("#jyvita_valitut_form input[value='" + tunnukset_array[x] + "']").remove();
				}
			}
			else {
				$("#jyvita_valitut_form input[value='" + $(this).val() + "']").remove();
			}
		}
	});
}

function nappi_onclick_confirm(message) {
	ok = confirm(message);

	return ok;
}

function bind_tarkista_tehtaan_saldot_click() {

	$('div.availability').css({
		'width': '15px',
		'height': '15px',
		'float': 'left',
		'background-color': 'transparent',
		'margin-left': '5px',
		'border-radius': '50%',
		'-webkit-border-radius': '50%',
		'-moz-border-radius': '50%'
	});

	var bgcolors = ['#E66','#FCF300','#5D2'];

	$('.tarkista_tehtaan_saldot').on('click', function() {

		$id = $(this).attr('id');
		$otunnus = $('#tilausnumero').val();
		$tuoteno = $('#'+$id+'_tuoteno').val();

		$('#'+$id+'_loading').html("<img src='../pics/loading_blue_small.gif' />");

		$.post('',
			{
			id: $id,
			otunnus: $otunnus,
			tuoteno: $tuoteno,
			ajax_toiminto: 'tarkista_tehtaan_saldot',
			no_head: 'yes',
			ohje: 'off'
			},
			function(return_value) {
				var data = jQuery.parseJSON(return_value);
				$('#'+data.id+'_loading').html('');

				if (data.saldo < 0) {
					$('#'+data.id+'_availability')
					.html("<img src='../pics/lullacons/alert.png' />");
				}
				else {
					$('#'+data.id+'_availability')
					.html('')
					.css({'background-color': bgcolors[data.saldo]})
					.show();
				}
			}
		);
	});
}
