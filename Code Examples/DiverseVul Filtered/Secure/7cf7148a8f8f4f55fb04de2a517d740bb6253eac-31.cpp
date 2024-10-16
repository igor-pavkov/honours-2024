PHP_MINFO_FUNCTION(miconv)
{
	zval *iconv_impl, *iconv_ver;

	iconv_impl = zend_get_constant_str("ICONV_IMPL", sizeof("ICONV_IMPL")-1);
	iconv_ver = zend_get_constant_str("ICONV_VERSION", sizeof("ICONV_VERSION")-1);

	php_info_print_table_start();
	php_info_print_table_row(2, "iconv support", "enabled");
	php_info_print_table_row(2, "iconv implementation", Z_STRVAL_P(iconv_impl));
	php_info_print_table_row(2, "iconv library version", Z_STRVAL_P(iconv_ver));
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}