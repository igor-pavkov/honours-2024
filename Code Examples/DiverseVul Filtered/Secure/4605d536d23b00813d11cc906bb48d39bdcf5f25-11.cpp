static void safe_php_register_variable(char *var, char *strval, int val_len, zval *track_vars_array, zend_bool override_protection TSRMLS_DC) /* {{{ */
{
	if (override_protection || !is_protected_variable(var TSRMLS_CC)) {
		php_register_variable_safe(var, strval, val_len, track_vars_array TSRMLS_CC);
	}
}