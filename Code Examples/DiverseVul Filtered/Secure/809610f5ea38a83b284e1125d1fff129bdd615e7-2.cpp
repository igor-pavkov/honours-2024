static void php_apache_request_dtor(request_rec *r TSRMLS_DC)
{
	php_request_shutdown(NULL);
}