PHP_FUNCTION(ldap_next_entry)
{
	zval *link, *result_entry;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry, *resultentry_next;
	LDAPMessage *entry_next;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &link, &result_entry) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, &result_entry, -1, "ldap result entry", le_result_entry);

	if ((entry_next = ldap_next_entry(ld->link, resultentry->data)) == NULL) {
		RETVAL_FALSE;
	} else {
		resultentry_next = emalloc(sizeof(ldap_resultentry));
		ZEND_REGISTER_RESOURCE(return_value, resultentry_next, le_result_entry);
		resultentry_next->id = resultentry->id;
		zend_list_addref(resultentry->id);
		resultentry_next->data = entry_next;
		resultentry_next->ber = NULL;
	}
}