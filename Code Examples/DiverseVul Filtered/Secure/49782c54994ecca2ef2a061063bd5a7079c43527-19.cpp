PHP_FUNCTION(ldap_first_reference)
{
	zval *link, *result;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	LDAPMessage *ldap_result, *entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &link, &result) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, &result, -1, "ldap result", le_result);

	if ((entry = ldap_first_reference(ld->link, ldap_result)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry = emalloc(sizeof(ldap_resultentry));
		ZEND_REGISTER_RESOURCE(return_value, resultentry, le_result_entry);
		resultentry->id = Z_LVAL_P(result);
		zend_list_addref(resultentry->id);
		resultentry->data = entry;
		resultentry->ber = NULL;
	}
}