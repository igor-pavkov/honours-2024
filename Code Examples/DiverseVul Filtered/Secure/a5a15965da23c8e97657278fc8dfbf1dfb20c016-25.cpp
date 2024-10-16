SPL_METHOD(SplFileObject, fgetc)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	char buf[2];
	int result;

	if(!intern->u.file.stream) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Object not initialized");
		return;
	}

	spl_filesystem_file_free_line(intern);

	result = php_stream_getc(intern->u.file.stream);

	if (result == EOF) {
		RETVAL_FALSE;
	} else {
		if (result == '\n') {
			intern->u.file.current_line_num++;
		}
		buf[0] = result;
		buf[1] = '\0';

		RETURN_STRINGL(buf, 1);
	}
} /* }}} */