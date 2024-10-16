PHP_METHOD(Phar, mapPhar)
{
	char *alias = NULL, *error;
	int alias_len = 0;
	long dataoffset = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!l", &alias, &alias_len, &dataoffset) == FAILURE) {
		return;
	}

	phar_request_initialize(TSRMLS_C);

	RETVAL_BOOL(phar_open_executed_filename(alias, alias_len, &error TSRMLS_CC) == SUCCESS);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
} /* }}} */