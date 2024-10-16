static void safe_php_register_variable_ex(char *var, zval *val, zval *track_vars_array, zend_bool override_protection TSRMLS_DC) /* {{{ */
{
	if (override_protection || !is_protected_variable(var TSRMLS_CC)) {
		php_register_variable_ex(var, val, track_vars_array TSRMLS_CC);
	}
}