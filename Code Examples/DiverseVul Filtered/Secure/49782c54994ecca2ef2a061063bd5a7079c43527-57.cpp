PHP_FUNCTION(ldap_sort)
{
	zval *link, *result;
	ldap_linkdata *ld;
	char *sortfilter;
	int sflen;
	zend_rsrc_list_entry *le;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrs", &link, &result, &sortfilter, &sflen) != SUCCESS) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	if (zend_hash_index_find(&EG(regular_list), Z_LVAL_P(result), (void **) &le) != SUCCESS || le->type != le_result) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Supplied resource is not a valid ldap result resource");
		RETURN_FALSE;
	}

	if (ldap_sort_entries(ld->link, (LDAPMessage **) &le->ptr, sflen ? sortfilter : NULL, strcmp) != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", ldap_err2string(errno));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}