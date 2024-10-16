static PHP_GINIT_FUNCTION(iconv)
{
#if defined(COMPILE_DL_ICONV) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	iconv_globals->input_encoding = NULL;
	iconv_globals->output_encoding = NULL;
	iconv_globals->internal_encoding = NULL;
}