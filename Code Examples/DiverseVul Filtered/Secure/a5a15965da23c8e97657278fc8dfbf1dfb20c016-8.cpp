SPL_METHOD(SplFileObject, fgetss)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	zval arg2;

	if(!intern->u.file.stream) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Object not initialized");
		return;
	}

	if (intern->u.file.max_line_len > 0) {
		ZVAL_LONG(&arg2, intern->u.file.max_line_len);
	} else {
		ZVAL_LONG(&arg2, 1024);
	}

	spl_filesystem_file_free_line(intern);
	intern->u.file.current_line_num++;

	FileFunctionCall(fgetss, ZEND_NUM_ARGS(), &arg2);
} /* }}} */