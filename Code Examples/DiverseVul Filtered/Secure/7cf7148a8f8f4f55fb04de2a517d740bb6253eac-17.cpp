PHP_MSHUTDOWN_FUNCTION(miconv)
{
	php_iconv_stream_filter_unregister_factory();
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}