static void php_apache_sapi_log_message_ex(char *msg, request_rec *r TSRMLS_DC)
{
	if (r) {
		ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, msg, r->filename);
	} else {
		php_apache_sapi_log_message(msg TSRMLS_CC);
	}
}