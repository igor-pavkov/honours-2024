PHP_FUNCTION(openssl_random_pseudo_bytes)
{
	zend_long buffer_length;
	zend_string *buffer = NULL;
	zval *zstrong_result_returned = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|z/", &buffer_length, &zstrong_result_returned) == FAILURE) {
		return;
	}

	if (zstrong_result_returned) {
		zval_dtor(zstrong_result_returned);
		ZVAL_FALSE(zstrong_result_returned);
	}

	if (buffer_length <= 0
#ifndef PHP_WIN32
		|| ZEND_LONG_INT_OVFL(buffer_length)
#endif
			) {
		RETURN_FALSE;
	}
	buffer = zend_string_alloc(buffer_length, 0);

#ifdef PHP_WIN32
	/* random/urandom equivalent on Windows */
	if (php_win32_get_random_bytes((unsigned char*)buffer->val, (size_t) buffer_length) == FAILURE){
		zend_string_release(buffer);
		if (zstrong_result_returned) {
			ZVAL_FALSE(zstrong_result_returned);
		}
		RETURN_FALSE;
	}
#else

	PHP_OPENSSL_CHECK_LONG_TO_INT(buffer_length, length);
	PHP_OPENSSL_RAND_ADD_TIME();
	/* FIXME loop if requested size > INT_MAX */
	if (RAND_bytes((unsigned char*)ZSTR_VAL(buffer), (int)buffer_length) <= 0) {
		zend_string_release(buffer);
		if (zstrong_result_returned) {
			ZVAL_FALSE(zstrong_result_returned);
		}
		RETURN_FALSE;
	} else {
		php_openssl_store_errors();
	}
#endif

	ZSTR_VAL(buffer)[buffer_length] = 0;
	RETVAL_STR(buffer);

	if (zstrong_result_returned) {
		ZVAL_BOOL(zstrong_result_returned, 1);
	}
}