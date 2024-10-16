SPL_METHOD(SplFileObject, key)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

/*	Do not read the next line to support correct counting with fgetc()
	if (!intern->current_line) {
		spl_filesystem_file_read_line(getThis(), intern, 1);
	} */
	RETURN_LONG(intern->u.file.current_line_num);
} /* }}} */