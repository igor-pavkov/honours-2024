SPL_METHOD(SplFileObject, fseek)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	zend_long pos, whence = SEEK_SET;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|l", &pos, &whence) == FAILURE) {
		return;
	}

	if(!intern->u.file.stream) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Object not initialized");
		return;
	}

	spl_filesystem_file_free_line(intern);
	RETURN_LONG(php_stream_seek(intern->u.file.stream, pos, (int)whence));
} /* }}} */