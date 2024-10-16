SPL_METHOD(SplFileInfo, getPathname)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	char *path;
	size_t path_len;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	path = spl_filesystem_object_get_pathname(intern, &path_len);
	if (path != NULL) {
		RETURN_STRINGL(path, path_len);
	} else {
		RETURN_FALSE;
	}
}