PHP_FUNCTION(ldap_get_dn)
{
	zval *link, *result_entry;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char *text;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &link, &result_entry) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, &result_entry, -1, "ldap result entry", le_result_entry);

	text = ldap_get_dn(ld->link, resultentry->data);
	if (text != NULL) {
		RETVAL_STRING(text, 1);
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP || WINDOWS
		ldap_memfree(text);
#else
		free(text);
#endif
	} else {
		RETURN_FALSE;
	}
}