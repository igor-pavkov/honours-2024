PHP_FUNCTION(ldap_get_option)
{
	zval *link, *retval;
	ldap_linkdata *ld;
	long option;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlz", &link, &option, &retval) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	switch (option) {
	/* options with int value */
	case LDAP_OPT_DEREF:
	case LDAP_OPT_SIZELIMIT:
	case LDAP_OPT_TIMELIMIT:
	case LDAP_OPT_PROTOCOL_VERSION:
	case LDAP_OPT_ERROR_NUMBER:
	case LDAP_OPT_REFERRALS:
#ifdef LDAP_OPT_RESTART
	case LDAP_OPT_RESTART:
#endif
		{
			int val;

			if (ldap_get_option(ld->link, option, &val)) {
				RETURN_FALSE;
			}
			zval_dtor(retval);
			ZVAL_LONG(retval, val);
		} break;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
	case LDAP_OPT_NETWORK_TIMEOUT:
		{
			struct timeval *timeout = NULL;

			if (ldap_get_option(ld->link, LDAP_OPT_NETWORK_TIMEOUT, (void *) &timeout)) {
				if (timeout) {
					ldap_memfree(timeout);
				}
				RETURN_FALSE;
			}
			if (!timeout) {
				RETURN_FALSE;
			}
			zval_dtor(retval);
			ZVAL_LONG(retval, timeout->tv_sec);
			ldap_memfree(timeout);
		} break;
#elif defined(LDAP_X_OPT_CONNECT_TIMEOUT)
	case LDAP_X_OPT_CONNECT_TIMEOUT:
		{
			int timeout;

			if (ldap_get_option(ld->link, LDAP_X_OPT_CONNECT_TIMEOUT, &timeout)) {
				RETURN_FALSE;
			}
			zval_dtor(retval);
			ZVAL_LONG(retval, (timeout / 1000));
		} break;
#endif
#ifdef LDAP_OPT_TIMEOUT
	case LDAP_OPT_TIMEOUT:
		{
			struct timeval *timeout = NULL;

			if (ldap_get_option(ld->link, LDAP_OPT_TIMEOUT, (void *) &timeout)) {
				if (timeout) {
					ldap_memfree(timeout);
				}
				RETURN_FALSE;
			}
			if (!timeout) {
				RETURN_FALSE;
			}
			zval_dtor(retval);
			ZVAL_LONG(retval, timeout->tv_sec);
			ldap_memfree(timeout);
		} break;
#endif
	/* options with string value */
	case LDAP_OPT_ERROR_STRING:
#ifdef LDAP_OPT_HOST_NAME
	case LDAP_OPT_HOST_NAME:
#endif
#ifdef HAVE_LDAP_SASL
	case LDAP_OPT_X_SASL_MECH:
	case LDAP_OPT_X_SASL_REALM:
	case LDAP_OPT_X_SASL_AUTHCID:
	case LDAP_OPT_X_SASL_AUTHZID:
#endif
#ifdef LDAP_OPT_MATCHED_DN
	case LDAP_OPT_MATCHED_DN:
#endif
		{
			char *val = NULL;

			if (ldap_get_option(ld->link, option, &val) || val == NULL || *val == '\0') {
				if (val) {
					ldap_memfree(val);
				}
				RETURN_FALSE;
			}
			zval_dtor(retval);
			ZVAL_STRING(retval, val, 1);
			ldap_memfree(val);
		} break;
/* options not implemented
	case LDAP_OPT_SERVER_CONTROLS:
	case LDAP_OPT_CLIENT_CONTROLS:
	case LDAP_OPT_API_INFO:
	case LDAP_OPT_API_FEATURE_INFO:
*/
	default:
		RETURN_FALSE;
	}
	RETURN_TRUE;
}