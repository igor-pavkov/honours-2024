PHP_FUNCTION(ldap_control_paged_result)
{
	long pagesize;
	zend_bool iscritical;
	zval *link;
	char *cookie = NULL;
	int cookie_len = 0;
	struct berval lcookie = { 0, NULL };
	ldap_linkdata *ld;
	LDAP *ldap;
	BerElement *ber = NULL;
	LDAPControl	ctrl, *ctrlsp[2];
	int rc, myargcount = ZEND_NUM_ARGS();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|bs", &link, &pagesize, &iscritical, &cookie, &cookie_len) != SUCCESS) {
		return;
	}

	if (Z_TYPE_P(link) == IS_NULL) {
		ldap = NULL;
	} else {
		ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
		ldap = ld->link;
	}

	ber = ber_alloc_t(LBER_USE_DER);
	if (ber == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to alloc BER encoding resources for paged results control");
		RETURN_FALSE;
	}

	ctrl.ldctl_iscritical = 0;

	switch (myargcount) {
		case 4:
			lcookie.bv_val = cookie;
			lcookie.bv_len = cookie_len;
			/* fallthru */
		case 3:
			ctrl.ldctl_iscritical = (int)iscritical;
			/* fallthru */
	}

	if (ber_printf(ber, "{iO}", (int)pagesize, &lcookie) == LBER_ERROR) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to BER printf paged results control");
		RETVAL_FALSE;
		goto lcpr_error_out;
	}
	rc = ber_flatten2(ber, &ctrl.ldctl_value, 0);
	if (rc == LBER_ERROR) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to BER encode paged results control");
		RETVAL_FALSE;
		goto lcpr_error_out;
	}

	ctrl.ldctl_oid = LDAP_CONTROL_PAGEDRESULTS;

	if (ldap) {
		/* directly set the option */
		ctrlsp[0] = &ctrl;
		ctrlsp[1] = NULL;

		rc = ldap_set_option(ldap, LDAP_OPT_SERVER_CONTROLS, ctrlsp);
		if (rc != LDAP_SUCCESS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to set paged results control: %s (%d)", ldap_err2string(rc), rc);
			RETVAL_FALSE;
			goto lcpr_error_out;
		}
		RETVAL_TRUE;
	} else {
		/* return a PHP control object */
		array_init(return_value);

		add_assoc_string(return_value, "oid", ctrl.ldctl_oid, 1);
		if (ctrl.ldctl_value.bv_len) {
			add_assoc_stringl(return_value, "value", ctrl.ldctl_value.bv_val, ctrl.ldctl_value.bv_len, 1);
		}
		if (ctrl.ldctl_iscritical) {
			add_assoc_bool(return_value, "iscritical", ctrl.ldctl_iscritical);
		}
	}

lcpr_error_out:
	if (ber != NULL) {
		ber_free(ber, 1);
	}
	return;
}