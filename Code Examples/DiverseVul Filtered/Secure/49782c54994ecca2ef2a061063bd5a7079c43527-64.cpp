PHP_FUNCTION(ldap_bind)
{
	zval *link;
	char *ldap_bind_dn = NULL, *ldap_bind_pw = NULL;
	int ldap_bind_dnlen, ldap_bind_pwlen;
	ldap_linkdata *ld;
	int rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|ss", &link, &ldap_bind_dn, &ldap_bind_dnlen, &ldap_bind_pw, &ldap_bind_pwlen) != SUCCESS) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	if (ldap_bind_dn != NULL && memchr(ldap_bind_dn, '\0', ldap_bind_dnlen) != NULL) {
		_set_lderrno(ld->link, LDAP_INVALID_CREDENTIALS);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "DN contains a null byte");
		RETURN_FALSE;
	}

	if (ldap_bind_pw != NULL && memchr(ldap_bind_pw, '\0', ldap_bind_pwlen) != NULL) {
		_set_lderrno(ld->link, LDAP_INVALID_CREDENTIALS);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Password contains a null byte");
		RETURN_FALSE;
	}

	{
#ifdef LDAP_API_FEATURE_X_OPENLDAP
		/* ldap_simple_bind_s() is deprecated, use ldap_sasl_bind_s() instead.
		 */
		struct berval   cred;

		cred.bv_val = ldap_bind_pw;
		cred.bv_len = ldap_bind_pw ? ldap_bind_pwlen : 0;
		rc = ldap_sasl_bind_s(ld->link, ldap_bind_dn, LDAP_SASL_SIMPLE, &cred,
				NULL, NULL,     /* no controls right now */
				NULL);	  /* we don't care about the server's credentials */
#else /* ! LDAP_API_FEATURE_X_OPENLDAP */
		rc = ldap_simple_bind_s(ld->link, ldap_bind_dn, ldap_bind_pw);
#endif /* ! LDAP_API_FEATURE_X_OPENLDAP */
	}
	if ( rc != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to bind to server: %s", ldap_err2string(rc));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}