static PHP_MINFO_FUNCTION(pcre)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "PCRE (Perl Compatible Regular Expressions) Support", "enabled" );
	php_info_print_table_row(2, "PCRE Library Version", pcre_version() );
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}