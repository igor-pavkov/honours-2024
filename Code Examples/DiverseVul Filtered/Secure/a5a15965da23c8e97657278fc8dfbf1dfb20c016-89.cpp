SPL_METHOD(SplFileObject, next)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_filesystem_file_free_line(intern);
	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_AHEAD)) {
		spl_filesystem_file_read_line(getThis(), intern, 1);
	}
	intern->u.file.current_line_num++;
} /* }}} */