PHP_MSHUTDOWN_FUNCTION(openssl)
{
	EVP_cleanup();

	php_unregister_url_stream_wrapper("https" TSRMLS_CC);
	php_unregister_url_stream_wrapper("ftps" TSRMLS_CC);

	php_stream_xport_unregister("ssl" TSRMLS_CC);
#ifndef OPENSSL_NO_SSL2
	php_stream_xport_unregister("sslv2" TSRMLS_CC);
#endif
	php_stream_xport_unregister("sslv3" TSRMLS_CC);
	php_stream_xport_unregister("tls" TSRMLS_CC);

	/* reinstate the default tcp handler */
	php_stream_xport_register("tcp", php_stream_generic_socket_factory TSRMLS_CC);

	return SUCCESS;
}