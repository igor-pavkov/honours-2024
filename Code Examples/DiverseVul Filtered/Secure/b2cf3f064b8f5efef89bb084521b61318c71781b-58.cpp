PHP_METHOD(Phar, mungServer)
{
	zval *mungvalues;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &mungvalues) == FAILURE) {
		return;
	}

	if (!zend_hash_num_elements(Z_ARRVAL_P(mungvalues))) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "No values passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME");
		return;
	}

	if (zend_hash_num_elements(Z_ARRVAL_P(mungvalues)) > 4) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Too many values passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME");
		return;
	}

	phar_request_initialize(TSRMLS_C);

	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(mungvalues)); SUCCESS == zend_hash_has_more_elements(Z_ARRVAL_P(mungvalues)); zend_hash_move_forward(Z_ARRVAL_P(mungvalues))) {
		zval **data = NULL;

		if (SUCCESS != zend_hash_get_current_data(Z_ARRVAL_P(mungvalues), (void **) &data)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "unable to retrieve array value in Phar::mungServer()");
			return;
		}

		if (Z_TYPE_PP(data) != IS_STRING) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Non-string value passed to Phar::mungServer(), expecting an array of any of these strings: PHP_SELF, REQUEST_URI, SCRIPT_FILENAME, SCRIPT_NAME");
			return;
		}

		if (Z_STRLEN_PP(data) == sizeof("PHP_SELF")-1 && !strncmp(Z_STRVAL_PP(data), "PHP_SELF", sizeof("PHP_SELF")-1)) {
			PHAR_GLOBALS->phar_SERVER_mung_list |= PHAR_MUNG_PHP_SELF;
		}

		if (Z_STRLEN_PP(data) == sizeof("REQUEST_URI")-1) {
			if (!strncmp(Z_STRVAL_PP(data), "REQUEST_URI", sizeof("REQUEST_URI")-1)) {
				PHAR_GLOBALS->phar_SERVER_mung_list |= PHAR_MUNG_REQUEST_URI;
			}
			if (!strncmp(Z_STRVAL_PP(data), "SCRIPT_NAME", sizeof("SCRIPT_NAME")-1)) {
				PHAR_GLOBALS->phar_SERVER_mung_list |= PHAR_MUNG_SCRIPT_NAME;
			}
		}

		if (Z_STRLEN_PP(data) == sizeof("SCRIPT_FILENAME")-1 && !strncmp(Z_STRVAL_PP(data), "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME")-1)) {
			PHAR_GLOBALS->phar_SERVER_mung_list |= PHAR_MUNG_SCRIPT_FILENAME;
		}
	}
}