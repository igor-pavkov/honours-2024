SPL_METHOD(FilesystemIterator, key)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (SPL_FILE_DIR_KEY(intern, SPL_FILE_DIR_KEY_AS_FILENAME)) {
		RETURN_STRING(intern->u.dir.entry.d_name);
	} else {
		spl_filesystem_object_get_file_name(intern);
		RETURN_STRINGL(intern->file_name, intern->file_name_len);
	}
}