SPL_METHOD(SplFileObject, seek)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	zend_long line_pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &line_pos) == FAILURE) {
		return;
	}
	if(!intern->u.file.stream) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Object not initialized");
		return;
	}

	if (line_pos < 0) {
		zend_throw_exception_ex(spl_ce_LogicException, 0, "Can't seek file %s to negative line " ZEND_LONG_FMT, intern->file_name, line_pos);
		RETURN_FALSE;
	}

	spl_filesystem_file_rewind(getThis(), intern);

	while(intern->u.file.current_line_num < line_pos) {
		if (spl_filesystem_file_read_line(getThis(), intern, 1) == FAILURE) {
			break;
		}
	}
} /* }}} */