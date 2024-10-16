PHP_FUNCTION(iconv_substr)
{
	char *charset = get_internal_encoding();
	size_t charset_len = 0;
	zend_string *str;
	zend_long offset, length = 0;

	php_iconv_err_t err;

	smart_str retval = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sl|ls",
		&str, &offset, &length,
		&charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() < 3) {
		length = ZSTR_LEN(str);
	}

	err = _php_iconv_substr(&retval, ZSTR_VAL(str), ZSTR_LEN(str), offset, length, charset);
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset);

	if (err == PHP_ICONV_ERR_SUCCESS && ZSTR_LEN(str) >= 0 && retval.s != NULL) {
		RETURN_NEW_STR(retval.s);
	}
	smart_str_free(&retval);
	RETURN_FALSE;
}