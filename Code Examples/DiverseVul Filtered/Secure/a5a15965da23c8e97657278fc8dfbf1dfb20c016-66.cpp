SPL_METHOD(SplFileInfo, getPath)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	char *path;
	size_t path_len;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

  	path = spl_filesystem_object_get_path(intern, &path_len);
	RETURN_STRINGL(path, path_len);
}