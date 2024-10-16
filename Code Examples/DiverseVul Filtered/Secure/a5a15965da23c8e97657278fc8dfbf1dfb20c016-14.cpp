SPL_METHOD(SplFileObject, valid)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		RETURN_BOOL(intern->u.file.current_line || !Z_ISUNDEF(intern->u.file.current_zval));
	} else {
		if(!intern->u.file.stream) {
			RETURN_FALSE;
		}
		RETVAL_BOOL(!php_stream_eof(intern->u.file.stream));
	}
} /* }}} */