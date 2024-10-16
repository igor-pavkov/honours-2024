SPL_METHOD(DirectoryIterator, isDot)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(spl_filesystem_is_dot(intern->u.dir.entry.d_name));
}