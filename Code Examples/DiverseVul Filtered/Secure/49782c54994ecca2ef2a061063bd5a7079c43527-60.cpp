PHP_FUNCTION(ldap_sasl_bind)
{
	zval *link;
	ldap_linkdata *ld;
	char *binddn = NULL;
	char *passwd = NULL;
	char *sasl_mech = NULL;
	char *sasl_realm = NULL;
	char *sasl_authz_id = NULL;
	char *sasl_authc_id = NULL;
	char *props = NULL;
	int rc, dn_len, passwd_len, mech_len, realm_len, authc_id_len, authz_id_len, props_len;
	php_ldap_bictx *ctx;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|sssssss", &link, &binddn, &dn_len, &passwd, &passwd_len, &sasl_mech, &mech_len, &sasl_realm, &realm_len, &sasl_authc_id, &authc_id_len, &sasl_authz_id, &authz_id_len, &props, &props_len) != SUCCESS) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	ctx = _php_sasl_setdefs(ld->link, sasl_mech, sasl_realm, sasl_authc_id, passwd, sasl_authz_id);

	if (props) {
		ldap_set_option(ld->link, LDAP_OPT_X_SASL_SECPROPS, props);
	}

	rc = ldap_sasl_interactive_bind_s(ld->link, binddn, ctx->mech, NULL, NULL, LDAP_SASL_QUIET, _php_sasl_interact, ctx);
	if (rc != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to bind to server: %s", ldap_err2string(rc));
		RETVAL_FALSE;
	} else {
		RETVAL_TRUE;
	}
	_php_sasl_freedefs(ctx);
}