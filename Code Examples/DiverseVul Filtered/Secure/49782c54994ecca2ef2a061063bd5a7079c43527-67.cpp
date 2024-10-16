int _ldap_rebind_proc(LDAP *ldap, const char *url, ber_tag_t req, ber_int_t msgid, void *params)
{
	ldap_linkdata *ld;
	int retval;
	zval *cb_url;
	zval **cb_args[2];
	zval *cb_retval;
	zval *cb_link = (zval *) params;
	TSRMLS_FETCH();

	ld = (ldap_linkdata *) zend_fetch_resource(&cb_link TSRMLS_CC, -1, "ldap link", NULL, 1, le_link);

	/* link exists and callback set? */
	if (ld == NULL || ld->rebindproc == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Link not found or no callback set");
		return LDAP_OTHER;
	}

	/* callback */
	MAKE_STD_ZVAL(cb_url);
	ZVAL_STRING(cb_url, estrdup(url), 0);
	cb_args[0] = &cb_link;
	cb_args[1] = &cb_url;
	if (call_user_function_ex(EG(function_table), NULL, ld->rebindproc, &cb_retval, 2, cb_args, 0, NULL TSRMLS_CC) == SUCCESS && cb_retval) {
		convert_to_long_ex(&cb_retval);
		retval = Z_LVAL_P(cb_retval);
		zval_ptr_dtor(&cb_retval);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "rebind_proc PHP callback failed");
		retval = LDAP_OTHER;
	}
	zval_dtor(cb_url);
	FREE_ZVAL(cb_url);
	return retval;
}