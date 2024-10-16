static void register_http_post_files_variable_ex(char *var, zval *val, zval *http_post_files, zend_bool override_protection TSRMLS_DC) /* {{{ */
{
	safe_php_register_variable_ex(var, val, http_post_files, override_protection TSRMLS_CC);
}