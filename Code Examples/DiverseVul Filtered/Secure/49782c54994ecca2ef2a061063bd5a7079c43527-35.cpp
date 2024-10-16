PHP_FUNCTION(ldap_unbind)
{
	zval *link;
	ldap_linkdata *ld;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &link) != SUCCESS) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	zend_list_delete(Z_LVAL_P(link));
	RETURN_TRUE;
}