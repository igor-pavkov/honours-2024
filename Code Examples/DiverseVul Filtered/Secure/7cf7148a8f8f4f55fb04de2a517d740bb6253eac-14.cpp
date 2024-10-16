PHP_FUNCTION(iconv_strlen)
{
	char *charset = get_internal_encoding();
	size_t charset_len = 0;
	zend_string *str;

	php_iconv_err_t err;

	size_t retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|s",
		&str, &charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	err = _php_iconv_strlen(&retval, ZSTR_VAL(str), ZSTR_LEN(str), charset);
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset);
	if (err == PHP_ICONV_ERR_SUCCESS) {
		RETVAL_LONG(retval);
	} else {
		RETVAL_FALSE;
	}
}