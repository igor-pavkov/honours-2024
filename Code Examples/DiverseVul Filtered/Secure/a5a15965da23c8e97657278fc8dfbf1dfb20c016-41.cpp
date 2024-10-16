SPL_METHOD(SplFileObject, getMaxLineLen)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG((zend_long)intern->u.file.max_line_len);
} /* }}} */