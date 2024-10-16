PHP_FUNCTION(ldap_rename)
{
	zval *link;
	ldap_linkdata *ld;
	int rc;
	char *dn, *newrdn, *newparent;
	int dn_len, newrdn_len, newparent_len;
	zend_bool deleteoldrdn;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsssb", &link, &dn, &dn_len, &newrdn, &newrdn_len, &newparent, &newparent_len, &deleteoldrdn) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	if (newparent_len == 0) {
		newparent = NULL;
	}

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP
	rc = ldap_rename_s(ld->link, dn, newrdn, newparent, deleteoldrdn, NULL, NULL);
#else
	if (newparent_len != 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "You are using old LDAP API, newparent must be the empty string, can only modify RDN");
		RETURN_FALSE;
	}
/* could support old APIs but need check for ldap_modrdn2()/ldap_modrdn() */
	rc = ldap_modrdn2_s(ld->link, dn, newrdn, deleteoldrdn);
#endif

	if (rc == LDAP_SUCCESS) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}