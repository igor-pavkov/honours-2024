SPL_METHOD(RecursiveDirectoryIterator, getSubPath)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (intern->u.dir.sub_path) {
		RETURN_STRINGL(intern->u.dir.sub_path, intern->u.dir.sub_path_len);
	} else {
		RETURN_EMPTY_STRING();
	}
}