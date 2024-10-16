PHP_FUNCTION(ldap_compare)
{
	zval *link;
	char *dn, *attr, *value;
	int dn_len, attr_len, value_len;
	ldap_linkdata *ld;
	int errno;
	struct berval lvalue;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsss", &link, &dn, &dn_len, &attr, &attr_len, &value, &value_len) != SUCCESS) {
		return;
	}

	lvalue.bv_val = value;
	lvalue.bv_len = value_len;

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	errno = ldap_compare_ext_s(ld->link, dn, attr, &lvalue, NULL, NULL);

	switch (errno) {
		case LDAP_COMPARE_TRUE:
			RETURN_TRUE;
			break;

		case LDAP_COMPARE_FALSE:
			RETURN_FALSE;
			break;
	}

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Compare: %s", ldap_err2string(errno));
	RETURN_LONG(-1);
}