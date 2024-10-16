SPL_METHOD(DirectoryIterator, valid)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(intern->u.dir.entry.d_name[0] != '\0');
}