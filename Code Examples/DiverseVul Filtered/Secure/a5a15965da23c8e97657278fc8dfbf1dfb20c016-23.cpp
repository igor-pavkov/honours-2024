SPL_METHOD(SplFileObject, setMaxLineLen)
{
	zend_long max_len;

	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &max_len) == FAILURE) {
		return;
	}

	if (max_len < 0) {
		zend_throw_exception_ex(spl_ce_DomainException, 0, "Maximum line length must be greater than or equal zero");
		return;
	}

	intern->u.file.max_line_len = max_len;
} /* }}} */