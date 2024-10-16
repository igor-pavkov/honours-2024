PHP_METHOD(Phar, loadPhar)
{
	char *fname, *alias = NULL, *error;
	size_t fname_len, alias_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p|s!", &fname, &fname_len, &alias, &alias_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_INT_OVFL(alias_len) || ZEND_SIZE_T_INT_OVFL(fname_len)) {
		RETURN_FALSE;
	}
	phar_request_initialize();

	RETVAL_BOOL(phar_open_from_filename(fname, (int)fname_len, alias, (int)alias_len, REPORT_ERRORS, NULL, &error) == SUCCESS);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
		efree(error);
	}
} /* }}} */