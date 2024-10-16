SPL_METHOD(SplFileObject, getFlags)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(intern->flags & SPL_FILE_OBJECT_MASK);
} /* }}} */