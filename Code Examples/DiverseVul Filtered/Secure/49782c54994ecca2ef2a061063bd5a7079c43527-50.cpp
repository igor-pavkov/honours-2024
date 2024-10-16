PHP_FUNCTION(ldap_control_paged_result_response)
{
	zval *link, *result, *cookie, *estimated;
	struct berval lcookie;
	int lestimated;
	ldap_linkdata *ld;
	LDAPMessage *ldap_result;
	LDAPControl **lserverctrls, *lctrl;
	BerElement *ber;
	ber_tag_t tag;
	int rc, lerrcode, myargcount = ZEND_NUM_ARGS();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr|zz", &link, &result, &cookie, &estimated) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, &result, -1, "ldap result", le_result);

	rc = ldap_parse_result(ld->link,
				ldap_result,
				&lerrcode,
				NULL,		/* matcheddn */
				NULL,		/* errmsg */
				NULL,		/* referrals */
				&lserverctrls,
				0);

	if (rc != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to parse result: %s (%d)", ldap_err2string(rc), rc);
		RETURN_FALSE;
	}

	if (lerrcode != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Result is: %s (%d)", ldap_err2string(lerrcode), lerrcode);
		RETURN_FALSE;
	}

	if (lserverctrls == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No server controls in result");
		RETURN_FALSE;
	}

	lctrl = ldap_control_find(LDAP_CONTROL_PAGEDRESULTS, lserverctrls, NULL);
	if (lctrl == NULL) {
		ldap_controls_free(lserverctrls);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No paged results control response in result");
		RETURN_FALSE;
	}

	ber = ber_init(&lctrl->ldctl_value);
	if (ber == NULL) {
		ldap_controls_free(lserverctrls);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to alloc BER decoding resources for paged results control response");
		RETURN_FALSE;
	}

	tag = ber_scanf(ber, "{io}", &lestimated, &lcookie);
	(void)ber_free(ber, 1);

	if (tag == LBER_ERROR) {
		ldap_controls_free(lserverctrls);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to decode paged results control response");
		RETURN_FALSE;
	}

	if (lestimated < 0) {
		ldap_controls_free(lserverctrls);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid paged results control response value");
		RETURN_FALSE;
	}

	ldap_controls_free(lserverctrls);
	if (myargcount == 4) {
		zval_dtor(estimated);
		ZVAL_LONG(estimated, lestimated);
	}

	zval_dtor(cookie);
 	if (lcookie.bv_len == 0) {
		ZVAL_EMPTY_STRING(cookie);
 	} else {
		ZVAL_STRINGL(cookie, lcookie.bv_val, lcookie.bv_len, 1);
 	}
 	ldap_memfree(lcookie.bv_val);

	RETURN_TRUE;
}