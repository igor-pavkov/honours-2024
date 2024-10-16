static void php_ldap_do_translate(INTERNAL_FUNCTION_PARAMETERS, int way)
{
	char *value;
	int result, ldap_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &value, &value_len) != SUCCESS) {
		return;
	}

	if (value_len == 0) {
		RETURN_FALSE;
	}

	if (way == 1) {
		result = ldap_8859_to_t61(&value, &value_len, 0);
	} else {
		result = ldap_t61_to_8859(&value, &value_len, 0);
	}

	if (result == LDAP_SUCCESS) {
		RETVAL_STRINGL(value, value_len, 1);
		free(value);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Conversion from iso-8859-1 to t61 failed: %s", ldap_err2string(result));
		RETVAL_FALSE;
	}
}