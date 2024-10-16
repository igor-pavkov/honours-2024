PHP_FUNCTION(ldap_set_rebind_proc)
{
	zval *link, *callback;
	ldap_linkdata *ld;
	char *callback_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &link, &callback) != SUCCESS) {
		RETURN_FALSE;
	}

	ZEND_FETCH_RESOURCE(ld, ldap_linkdata *, &link, -1, "ldap link", le_link);

	if (Z_TYPE_P(callback) == IS_STRING && Z_STRLEN_P(callback) == 0) {
		/* unregister rebind procedure */
		if (ld->rebindproc != NULL) {
			zval_dtor(ld->rebindproc);
			FREE_ZVAL(ld->rebindproc);
			ld->rebindproc = NULL;
			ldap_set_rebind_proc(ld->link, NULL, NULL);
		}
		RETURN_TRUE;
	}

	/* callable? */
	if (!zend_is_callable(callback, 0, &callback_name TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Two arguments expected for '%s' to be a valid callback", callback_name);
		efree(callback_name);
		RETURN_FALSE;
	}
	efree(callback_name);

	/* register rebind procedure */
	if (ld->rebindproc == NULL) {
		ldap_set_rebind_proc(ld->link, _ldap_rebind_proc, (void *) link);
	} else {
		zval_dtor(ld->rebindproc);
	}

	ALLOC_ZVAL(ld->rebindproc);
	*ld->rebindproc = *callback;
	zval_copy_ctor(ld->rebindproc);
	RETURN_TRUE;
}