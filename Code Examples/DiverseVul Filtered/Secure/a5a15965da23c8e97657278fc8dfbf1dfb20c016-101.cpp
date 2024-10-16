SPL_METHOD(SplFileObject, current)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if(!intern->u.file.stream) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Object not initialized");
		return;
	}

	if (!intern->u.file.current_line && Z_ISUNDEF(intern->u.file.current_zval)) {
		spl_filesystem_file_read_line(getThis(), intern, 1);
	}
	if (intern->u.file.current_line && (!SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV) || Z_ISUNDEF(intern->u.file.current_zval))) {
		RETURN_STRINGL(intern->u.file.current_line, intern->u.file.current_line_len);
	} else if (!Z_ISUNDEF(intern->u.file.current_zval)) {
		zval *value = &intern->u.file.current_zval;

		ZVAL_DEREF(value);
		ZVAL_COPY(return_value, value);
		return;
	}
	RETURN_FALSE;
} /* }}} */