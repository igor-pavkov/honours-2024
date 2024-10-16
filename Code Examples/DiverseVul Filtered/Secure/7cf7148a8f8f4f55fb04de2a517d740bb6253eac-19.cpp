PHP_FUNCTION(iconv_mime_decode)
{
	zend_string *encoded_str;
	char *charset = get_internal_encoding();
	size_t charset_len = 0;
	zend_long mode = 0;

	smart_str retval = {0};

	php_iconv_err_t err;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|ls",
		&encoded_str, &mode, &charset, &charset_len) == FAILURE) {

		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	err = _php_iconv_mime_decode(&retval, ZSTR_VAL(encoded_str), ZSTR_LEN(encoded_str), charset, NULL, (int)mode);
	_php_iconv_show_error(err, charset, "???");

	if (err == PHP_ICONV_ERR_SUCCESS) {
		if (retval.s != NULL) {
			RETVAL_STR(retval.s);
		} else {
			RETVAL_EMPTY_STRING();
		}
	} else {
		smart_str_free(&retval);
		RETVAL_FALSE;
	}
}