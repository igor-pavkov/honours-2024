PHP_FUNCTION(openssl_random_pseudo_bytes)
{
	long buffer_length;
	unsigned char *buffer = NULL;
	zval *zstrong_result_returned = NULL;
	int strong_result = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|z", &buffer_length, &zstrong_result_returned) == FAILURE) {
		return;
	}

	if (buffer_length <= 0) {
		RETURN_FALSE;
	}

	if (zstrong_result_returned) {
		zval_dtor(zstrong_result_returned);
		ZVAL_BOOL(zstrong_result_returned, 0);
	}

	buffer = emalloc(buffer_length + 1);

	if ((strong_result = RAND_pseudo_bytes(buffer, buffer_length)) < 0) {
		efree(buffer);
		RETURN_FALSE;
	}

	buffer[buffer_length] = 0;
	RETVAL_STRINGL((char *)buffer, buffer_length, 0);

	if (zstrong_result_returned) {
		ZVAL_BOOL(zstrong_result_returned, strong_result);
	}
}