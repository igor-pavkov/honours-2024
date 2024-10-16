static double php_apache_sapi_get_request_time(TSRMLS_D)
{
	php_struct *ctx = SG(server_context);
	return ((double) apr_time_as_msec(ctx->r->request_time)) / 1000.0;
}