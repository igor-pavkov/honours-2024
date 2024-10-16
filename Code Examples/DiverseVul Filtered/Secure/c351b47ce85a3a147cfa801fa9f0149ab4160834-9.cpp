static PHP_FUNCTION(preg_last_error)
{
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	RETURN_LONG(PCRE_G(error_code));
}