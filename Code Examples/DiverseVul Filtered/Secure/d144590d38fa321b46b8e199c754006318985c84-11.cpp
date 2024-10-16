PHP_METHOD(Phar, loadPhar)
{
	char *fname, *alias = NULL, *error;
	int fname_len, alias_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|s!", &fname, &fname_len, &alias, &alias_len) == FAILURE) {
		return;
	}

	phar_request_initialize(TSRMLS_C);

	RETVAL_BOOL(phar_open_from_filename(fname, fname_len, alias, alias_len, REPORT_ERRORS, NULL, &error TSRMLS_CC) == SUCCESS);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
} /* }}} */