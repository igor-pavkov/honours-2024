SPL_METHOD(DirectoryIterator, key)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (intern->u.dir.dirp) {
		RETURN_LONG(intern->u.dir.index);
	} else {
		RETURN_FALSE;
	}
}