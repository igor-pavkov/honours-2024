PHP_FUNCTION(ldap_parse_result)
{
	zval *link, *result, *errcode, *matcheddn, *errmsg, *referrals;
	ldap_linkdata *ld;
	LDAPMessage *ldap_result;
	char **lreferrals, **refp;
	char *lmatcheddn, *lerrmsg;
	int rc, lerrcode, myargcount = ZEND_NUM_ARGS();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrz|zzz", &link, &result, &errcode, &matcheddn, &errmsg, &referrals) != SUCCESS) {
		return;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);
	ZEND_FETCH_RESOURCE(ldap_result, LDAPMessage *, &result, -1, "ldap result", le_result);

	rc = ldap_parse_result(ld->link, ldap_result, &lerrcode,
				myargcount > 3 ? &lmatcheddn : NULL,
				myargcount > 4 ? &lerrmsg : NULL,
				myargcount > 5 ? &lreferrals : NULL,
				NULL /* &serverctrls */,
				0);
	if (rc != LDAP_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to parse result: %s", ldap_err2string(rc));
		RETURN_FALSE;
	}

	zval_dtor(errcode);
	ZVAL_LONG(errcode, lerrcode);

	/* Reverse -> fall through */
	switch (myargcount) {
		case 6:
			zval_dtor(referrals);
			array_init(referrals);
			if (lreferrals != NULL) {
				refp = lreferrals;
				while (*refp) {
					add_next_index_string(referrals, *refp, 1);
					refp++;
				}
				ldap_memvfree((void**)lreferrals);
			}
		case 5:
			zval_dtor(errmsg);
			if (lerrmsg == NULL) {
				ZVAL_EMPTY_STRING(errmsg);
			} else {
				ZVAL_STRING(errmsg, lerrmsg, 1);
				ldap_memfree(lerrmsg);
			}
		case 4:
			zval_dtor(matcheddn);
			if (lmatcheddn == NULL) {
				ZVAL_EMPTY_STRING(matcheddn);
			} else {
				ZVAL_STRING(matcheddn, lmatcheddn, 1);
				ldap_memfree(lmatcheddn);
			}
	}
	RETURN_TRUE;
}