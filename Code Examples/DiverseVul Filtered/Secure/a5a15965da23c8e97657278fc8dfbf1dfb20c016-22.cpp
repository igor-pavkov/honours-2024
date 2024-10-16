SPL_METHOD(SplFileObject, fwrite)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	char *str;
	size_t str_len;
	zend_long length = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &str, &str_len, &length) == FAILURE) {
		return;
	}

	if(!intern->u.file.stream) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Object not initialized");
		return;
	}

	if (ZEND_NUM_ARGS() > 1) {
		if (length >= 0) {
			str_len = MIN((size_t)length, str_len);
		} else {
			/* Negative length given, nothing to write */
			str_len = 0;
		}
	}
	if (!str_len) {
		RETURN_LONG(0);
	}

	RETURN_LONG(php_stream_write(intern->u.file.stream, str, str_len));
} /* }}} */