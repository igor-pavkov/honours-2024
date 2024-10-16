PHP_FUNCTION(ldap_free_result)
{
	zval *result;
	LDAPMessage *ldap_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &result) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, &result, -1, "ldap result", le_result);

	zend_list_delete(Z_LVAL_P(result));  /* Delete list entry */
	RETVAL_TRUE;
}