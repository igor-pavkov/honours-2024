SPL_METHOD(SplFileObject, setFlags)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &intern->flags) == FAILURE) {
		return;
	}
} /* }}} */