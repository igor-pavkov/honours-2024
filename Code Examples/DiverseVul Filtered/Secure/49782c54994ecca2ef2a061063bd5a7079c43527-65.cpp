PHP_FUNCTION(ldap_start_tls)
{
	zval *link;
	ldap_linkdata *ld;
	int rc, protocol = LDAP_VERSION3;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &link) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	if (((rc = ldap_set_option(ld->link, LDAP_OPT_PROTOCOL_VERSION, &protocol)) != LDAP_SUCCESS) ||
		((rc = ldap_start_tls_s(ld->link, NULL, NULL)) != LDAP_SUCCESS)
	) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Unable to start TLS: %s", ldap_err2string(rc));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}