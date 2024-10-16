SPL_METHOD(SplFileInfo, getFilename)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	size_t path_len;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_filesystem_object_get_path(intern, &path_len);

	if (path_len && path_len < intern->file_name_len) {
		RETURN_STRINGL(intern->file_name + path_len + 1, intern->file_name_len - (path_len + 1));
	} else {
		RETURN_STRINGL(intern->file_name, intern->file_name_len);
	}
}