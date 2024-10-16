PHP_FUNCTION(ldap_count_entries)
{
	zval *link, *result;
	ldap_linkdata *ld;
	LDAPMessage *ldap_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &link, &result) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, &result, -1, "ldap result", le_result);

	RETURN_LONG(ldap_count_entries(ld->link, ldap_result));
}