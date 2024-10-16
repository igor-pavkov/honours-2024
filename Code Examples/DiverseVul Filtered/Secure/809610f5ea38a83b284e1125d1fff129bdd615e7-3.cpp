static void php_apache_sapi_log_message(char *msg TSRMLS_DC)
{
	php_struct *ctx;

	ctx = SG(server_context);

	if (ctx == NULL) { /* we haven't initialized our ctx yet, oh well */
		ap_log_error(APLOG_MARK, APLOG_ERR | APLOG_STARTUP, 0, NULL, "%s", msg);
	} else {
		ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, ctx->r, "%s", msg);
	}
}