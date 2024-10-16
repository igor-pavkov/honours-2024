PHP_FUNCTION(iconv_strrpos)
{
	char *charset = get_internal_encoding();
	size_t charset_len = 0;
	zend_string *haystk;
	zend_string *ndl;

	php_iconv_err_t err;

	size_t retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|s",
		&haystk, &ndl,
		&charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (ZSTR_LEN(ndl) < 1) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	err = _php_iconv_strpos(&retval, ZSTR_VAL(haystk), ZSTR_LEN(haystk), ZSTR_VAL(ndl), ZSTR_LEN(ndl),
	                        -1, charset);
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset);

	if (err == PHP_ICONV_ERR_SUCCESS && retval != (size_t)-1) {
		RETVAL_LONG((zend_long)retval);
	} else {
		RETVAL_FALSE;
	}
}