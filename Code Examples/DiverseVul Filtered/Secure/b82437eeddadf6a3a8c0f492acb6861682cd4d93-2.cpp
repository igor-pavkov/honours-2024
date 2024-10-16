PHP_MINFO_FUNCTION(exif)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "EXIF Support", "enabled");
	php_info_print_table_row(2, "Supported EXIF Version", "0220");
	php_info_print_table_row(2, "Supported filetypes", "JPEG, TIFF");

	if (zend_hash_str_exists(&module_registry, "mbstring", sizeof("mbstring")-1)) {
		php_info_print_table_row(2, "Multibyte decoding support using mbstring", "enabled");
	} else {
		php_info_print_table_row(2, "Multibyte decoding support using mbstring", "disabled");
	}

	php_info_print_table_row(2, "Extended EXIF tag formats", "Canon, Casio, Fujifilm, Nikon, Olympus, Samsung, Panasonic, DJI, Sony, Pentax, Minolta, Sigma, Foveon, Kyocera, Ricoh, AGFA, Epson");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}