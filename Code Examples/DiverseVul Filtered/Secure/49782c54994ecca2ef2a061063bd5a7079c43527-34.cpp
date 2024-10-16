PHP_FUNCTION(ldap_delete)
{
	zval *link;
	ldap_linkdata *ld;
	char *dn;
	int rc, dn_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &link, &dn, &dn_len) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	if ((rc = ldap_delete_ext_s(ld->link, dn, NULL, NULL)) != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Delete: %s", ldap_err2string(rc));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}