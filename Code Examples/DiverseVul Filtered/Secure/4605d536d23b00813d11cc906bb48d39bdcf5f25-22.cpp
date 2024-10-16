static zend_bool is_protected_variable(char *varname TSRMLS_DC) /* {{{ */
{
	normalize_protected_variable(varname TSRMLS_CC);
	return zend_hash_exists(&PG(rfc1867_protected_variables), varname, strlen(varname)+1);
}