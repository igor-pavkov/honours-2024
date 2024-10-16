PHP_FUNCTION(ldap_first_attribute)
{
	zval *link, *result_entry;
	ldap_linkdata *ld;
	ldap_resultentry *resultentry;
	char *attribute;
	long dummy_ber;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr|l", &link, &result_entry, &dummy_ber) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(resultentry, ldap_resultentry *, &result_entry, -1, "ldap result entry", le_result_entry);

	if ((attribute = ldap_first_attribute(ld->link, resultentry->data, &resultentry->ber)) == NULL) {
		RETURN_FALSE;
	} else {
		RETVAL_STRING(attribute, 1);
#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP || WINDOWS
		ldap_memfree(attribute);
#endif
	}
}