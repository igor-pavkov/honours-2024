PHP_MSHUTDOWN_FUNCTION(openssl)
{
	EVP_cleanup();

#if OPENSSL_VERSION_NUMBER >= 0x00090805f
	/* prevent accessing locking callback from unloaded extension */
	CRYPTO_set_locking_callback(NULL);
	/* free allocated error strings */
	ERR_free_strings();
#endif

	php_unregister_url_stream_wrapper("https" TSRMLS_CC);
	php_unregister_url_stream_wrapper("ftps" TSRMLS_CC);

	php_stream_xport_unregister("ssl" TSRMLS_CC);
#ifndef OPENSSL_NO_SSL2
	php_stream_xport_unregister("sslv2" TSRMLS_CC);
#endif
	php_stream_xport_unregister("sslv3" TSRMLS_CC);
	php_stream_xport_unregister("tls" TSRMLS_CC);
	php_stream_xport_unregister("tlsv1.0" TSRMLS_CC);
#if OPENSSL_VERSION_NUMBER >= 0x10001001L
	php_stream_xport_unregister("tlsv1.1" TSRMLS_CC);
	php_stream_xport_unregister("tlsv1.2" TSRMLS_CC);
#endif

	/* reinstate the default tcp handler */
	php_stream_xport_register("tcp", php_stream_generic_socket_factory TSRMLS_CC);

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}