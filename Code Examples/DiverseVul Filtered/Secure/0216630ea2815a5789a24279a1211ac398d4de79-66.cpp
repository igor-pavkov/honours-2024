PHP_GINIT_FUNCTION(openssl)
{
#if defined(COMPILE_DL_OPENSSL) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	openssl_globals->errors = NULL;
}