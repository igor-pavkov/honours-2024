SPL_METHOD(SplFileObject, fgets)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if(!intern->u.file.stream) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Object not initialized");
		return;
	}

	if (spl_filesystem_file_read(intern, 0) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(intern->u.file.current_line, intern->u.file.current_line_len);
} /* }}} */