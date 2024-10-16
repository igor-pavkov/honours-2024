SPL_METHOD(DirectoryIterator, getFilename)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_STRING(intern->u.dir.entry.d_name);
}