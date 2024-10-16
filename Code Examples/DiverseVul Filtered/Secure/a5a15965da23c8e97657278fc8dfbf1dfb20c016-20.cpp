SPL_METHOD(SplFileObject, rewind)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_filesystem_file_rewind(getThis(), intern);
} /* }}} */