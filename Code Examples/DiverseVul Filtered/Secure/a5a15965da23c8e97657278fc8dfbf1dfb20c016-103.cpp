SPL_METHOD(SplFileObject, ftruncate)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	zend_long size;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &size) == FAILURE) {
		return;
	}

	if(!intern->u.file.stream) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Object not initialized");
		return;
	}

	if (!php_stream_truncate_supported(intern->u.file.stream)) {
		zend_throw_exception_ex(spl_ce_LogicException, 0, "Can't truncate file %s", intern->file_name);
		RETURN_FALSE;
	}

	RETURN_BOOL(0 == php_stream_truncate_set_size(intern->u.file.stream, size));
} /* }}} */