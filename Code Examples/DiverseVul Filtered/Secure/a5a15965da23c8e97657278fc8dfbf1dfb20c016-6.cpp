SPL_METHOD(SplFileObject, fread)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	zend_long length = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &length) == FAILURE) {
		return;
	}

	if(!intern->u.file.stream) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Object not initialized");
		return;
	}

	if (length <= 0) {
		php_error_docref(NULL, E_WARNING, "Length parameter must be greater than 0");
		RETURN_FALSE;
	}

	ZVAL_NEW_STR(return_value, zend_string_alloc(length, 0));
	Z_STRLEN_P(return_value) = php_stream_read(intern->u.file.stream, Z_STRVAL_P(return_value), length);

	/* needed because recv/read/gzread doesnt put a null at the end*/
	Z_STRVAL_P(return_value)[Z_STRLEN_P(return_value)] = 0;
}