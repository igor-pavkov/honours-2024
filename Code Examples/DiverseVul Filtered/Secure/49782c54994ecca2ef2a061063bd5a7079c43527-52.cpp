PHP_FUNCTION(ldap_connect)
{
	char *host = NULL;
	int hostlen;
	long port = LDAP_PORT;
#ifdef HAVE_ORALDAP
	char *wallet = NULL, *walletpasswd = NULL;
	int walletlen = 0, walletpasswdlen = 0;
	long authmode = GSLC_SSL_NO_AUTH;
	int ssl=0;
#endif
	ldap_linkdata *ld;
	LDAP *ldap = NULL;

#ifdef HAVE_ORALDAP
	if (ZEND_NUM_ARGS() == 3 || ZEND_NUM_ARGS() == 4) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|slssl", &host, &hostlen, &port, &wallet, &walletlen, &walletpasswd, &walletpasswdlen, &authmode) != SUCCESS) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() == 5) {
		ssl = 1;
	}
#else
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sl", &host, &hostlen, &port) != SUCCESS) {
		RETURN_FALSE;
	}
#endif
	if (!port) {
		port = LDAP_PORT;
	}

	if (LDAPG(max_links) != -1 && LDAPG(num_links) >= LDAPG(max_links)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Too many open links (%ld)", LDAPG(num_links));
		RETURN_FALSE;
	}

	ld = ecalloc(1, sizeof(ldap_linkdata));

	{
		int rc = LDAP_SUCCESS;
		char	*url = host;
		if (!ldap_is_ldap_url(url)) {
			int	urllen = hostlen + sizeof( "ldap://:65535" );

			if (port <= 0 || port > 65535) {
				efree(ld);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid port number: %ld", port);
				RETURN_FALSE;
			}

			url = emalloc(urllen);
			if (host && (strchr(host, ':') != NULL)) {
				/* Legacy support for host:port */
				snprintf( url, urllen, "ldap://%s", host );
			} else {
				snprintf( url, urllen, "ldap://%s:%ld", host ? host : "", port );
			}
		}

#ifdef LDAP_API_FEATURE_X_OPENLDAP
		/* ldap_init() is deprecated, use ldap_initialize() instead.
		 */
		rc = ldap_initialize(&ldap, url);
#else /* ! LDAP_API_FEATURE_X_OPENLDAP */
		/* ldap_init does not support URLs.
		 * We must try the original host and port information.
		 */
		ldap = ldap_init(host, port);
		if (ldap == NULL) {
			efree(ld);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not create session handle");
			RETURN_FALSE;
		}
#endif /* ! LDAP_API_FEATURE_X_OPENLDAP */
		if (url != host) {
			efree(url);
		}
		if (rc != LDAP_SUCCESS) {
			efree(ld);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not create session handle: %s", ldap_err2string(rc));
			RETURN_FALSE;
		}
	}

	if (ldap == NULL) {
		efree(ld);
		RETURN_FALSE;
	} else {
#ifdef HAVE_ORALDAP
		if (ssl) {
			if (ldap_init_SSL(&ldap->ld_sb, wallet, walletpasswd, authmode)) {
				efree(ld);
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "SSL init failed");
				RETURN_FALSE;
			}
		}
#endif
		LDAPG(num_links)++;
		ld->link = ldap;
		ZEND_REGISTER_RESOURCE(return_value, ld, le_link);
	}

}