PHP_FUNCTION(ldap_set_option)
{
	zval *link, **newval;
	ldap_linkdata *ld;
	LDAP *ldap;
	long option;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zlZ", &link, &option, &newval) != SUCCESS) {
		return;
	}

	if (Z_TYPE_P(link) == IS_NULL) {
		ldap = NULL;
	} else {
		ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
		ldap = ld->link;
	}

	switch (option) {
	/* options with int value */
	case LDAP_OPT_DEREF:
	case LDAP_OPT_SIZELIMIT:
	case LDAP_OPT_TIMELIMIT:
	case LDAP_OPT_PROTOCOL_VERSION:
	case LDAP_OPT_ERROR_NUMBER:
#ifdef LDAP_OPT_DEBUG_LEVEL
	case LDAP_OPT_DEBUG_LEVEL:
#endif
		{
			int val;

			convert_to_long_ex(newval);
			val = Z_LVAL_PP(newval);
			if (ldap_set_option(ldap, option, &val)) {
				RETURN_FALSE;
			}
		} break;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
	case LDAP_OPT_NETWORK_TIMEOUT:
		{
			struct timeval timeout;

			convert_to_long_ex(newval);
			timeout.tv_sec = Z_LVAL_PP(newval);
			timeout.tv_usec = 0;
			if (ldap_set_option(ldap, LDAP_OPT_NETWORK_TIMEOUT, (void *) &timeout)) {
				RETURN_FALSE;
			}
		} break;
#elif defined(LDAP_X_OPT_CONNECT_TIMEOUT)
	case LDAP_X_OPT_CONNECT_TIMEOUT:
		{
			int timeout;

			convert_to_long_ex(newval);
			timeout = 1000 * Z_LVAL_PP(newval); /* Convert to milliseconds */
			if (ldap_set_option(ldap, LDAP_X_OPT_CONNECT_TIMEOUT, &timeout)) {
				RETURN_FALSE;
			}
		} break;
#endif
#ifdef LDAP_OPT_TIMEOUT
	case LDAP_OPT_TIMEOUT:
		{
			struct timeval timeout;

			convert_to_long_ex(newval);
			timeout.tv_sec = Z_LVAL_PP(newval);
			timeout.tv_usec = 0;
			if (ldap_set_option(ldap, LDAP_OPT_TIMEOUT, (void *) &timeout)) {
				RETURN_FALSE;
			}
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
			char *val;
			convert_to_string_ex(newval);
			val = Z_STRVAL_PP(newval);
			if (ldap_set_option(ldap, option, val)) {
				RETURN_FALSE;
			}
		} break;
		/* options with boolean value */
	case LDAP_OPT_REFERRALS:
#ifdef LDAP_OPT_RESTART
	case LDAP_OPT_RESTART:
#endif
		{
			void *val;
			convert_to_boolean_ex(newval);
			val = Z_LVAL_PP(newval)
				? LDAP_OPT_ON : LDAP_OPT_OFF;
			if (ldap_set_option(ldap, option, val)) {
				RETURN_FALSE;
			}
		} break;
		/* options with control list value */
	case LDAP_OPT_SERVER_CONTROLS:
	case LDAP_OPT_CLIENT_CONTROLS:
		{
			LDAPControl *ctrl, **ctrls, **ctrlp;
			zval **ctrlval, **val;
			int ncontrols;
			char error=0;

			if ((Z_TYPE_PP(newval) != IS_ARRAY) || !(ncontrols = zend_hash_num_elements(Z_ARRVAL_PP(newval)))) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expected non-empty array value for this option");
				RETURN_FALSE;
			}
			ctrls = safe_emalloc((1 + ncontrols), sizeof(*ctrls), 0);
			*ctrls = NULL;
			ctrlp = ctrls;
			zend_hash_internal_pointer_reset(Z_ARRVAL_PP(newval));
			while (zend_hash_get_current_data(Z_ARRVAL_PP(newval), (void**)&ctrlval) == SUCCESS) {
				if (Z_TYPE_PP(ctrlval) != IS_ARRAY) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "The array value must contain only arrays, where each array is a control");
					error = 1;
					break;
				}
				if (zend_hash_find(Z_ARRVAL_PP(ctrlval), "oid", sizeof("oid"), (void **) &val) != SUCCESS) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Control must have an oid key");
					error = 1;
					break;
				}
				ctrl = *ctrlp = emalloc(sizeof(**ctrlp));
				convert_to_string_ex(val);
				ctrl->ldctl_oid = Z_STRVAL_PP(val);
				if (zend_hash_find(Z_ARRVAL_PP(ctrlval), "value", sizeof("value"), (void **) &val) == SUCCESS) {
					convert_to_string_ex(val);
					ctrl->ldctl_value.bv_val = Z_STRVAL_PP(val);
					ctrl->ldctl_value.bv_len = Z_STRLEN_PP(val);
				} else {
					ctrl->ldctl_value.bv_val = NULL;
					ctrl->ldctl_value.bv_len = 0;
				}
				if (zend_hash_find(Z_ARRVAL_PP(ctrlval), "iscritical", sizeof("iscritical"), (void **) &val) == SUCCESS) {
					convert_to_boolean_ex(val);
					ctrl->ldctl_iscritical = Z_BVAL_PP(val);
				} else {
					ctrl->ldctl_iscritical = 0;
				}

				++ctrlp;
				*ctrlp = NULL;
				zend_hash_move_forward(Z_ARRVAL_PP(newval));
			}
			if (!error) {
				error = ldap_set_option(ldap, option, ctrls);
			}
			ctrlp = ctrls;
			while (*ctrlp) {
				efree(*ctrlp);
				ctrlp++;
			}
			efree(ctrls);
			if (error) {
				RETURN_FALSE;
			}
		} break;
	default:
		RETURN_FALSE;
	}
	RETURN_TRUE;
}