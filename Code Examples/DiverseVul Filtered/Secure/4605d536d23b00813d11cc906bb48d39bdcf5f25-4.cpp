static void add_protected_variable(char *varname TSRMLS_DC) /* {{{ */
{
	int dummy = 1;

	normalize_protected_variable(varname TSRMLS_CC);
	zend_hash_add(&PG(rfc1867_protected_variables), varname, strlen(varname)+1, &dummy, sizeof(int), NULL);
}