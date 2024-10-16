PHP_METHOD(Phar, mapPhar)
{
	char *alias = NULL, *error;
	size_t alias_len = 0;
	zend_long dataoffset = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s!l", &alias, &alias_len, &dataoffset) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_INT_OVFL(alias_len)) {
		RETURN_FALSE;
	}
	phar_request_initialize();

	RETVAL_BOOL(phar_open_executed_filename(alias, (int)alias_len, &error) == SUCCESS);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
		efree(error);
	}
} /* }}} */