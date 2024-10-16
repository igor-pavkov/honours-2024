SPL_METHOD(FilesystemIterator, current)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (SPL_FILE_DIR_CURRENT(intern, SPL_FILE_DIR_CURRENT_AS_PATHNAME)) {
		spl_filesystem_object_get_file_name(intern);
		RETURN_STRINGL(intern->file_name, intern->file_name_len);
	} else if (SPL_FILE_DIR_CURRENT(intern, SPL_FILE_DIR_CURRENT_AS_FILEINFO)) {
		spl_filesystem_object_get_file_name(intern);
		spl_filesystem_object_create_type(0, intern, SPL_FS_INFO, NULL, return_value);
	} else {
		ZVAL_OBJ(return_value, Z_OBJ_P(getThis()));
		Z_ADDREF_P(return_value);
		/*RETURN_STRING(intern->u.dir.entry.d_name, 1);*/
	}
}