SPL_METHOD(RecursiveDirectoryIterator, getChildren)
{
	zval zpath, zflags;
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	spl_filesystem_object *subdir;
	char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_filesystem_object_get_file_name(intern);

	ZVAL_LONG(&zflags, intern->flags);
	ZVAL_STRINGL(&zpath, intern->file_name, intern->file_name_len);
	spl_instantiate_arg_ex2(Z_OBJCE_P(getThis()), return_value, &zpath, &zflags);
	zval_ptr_dtor(&zpath);
	zval_ptr_dtor(&zflags);

	subdir = Z_SPLFILESYSTEM_P(return_value);
	if (subdir) {
		if (intern->u.dir.sub_path && intern->u.dir.sub_path[0]) {
			subdir->u.dir.sub_path_len = spprintf(&subdir->u.dir.sub_path, 0, "%s%c%s", intern->u.dir.sub_path, slash, intern->u.dir.entry.d_name);
		} else {
			subdir->u.dir.sub_path_len = strlen(intern->u.dir.entry.d_name);
			subdir->u.dir.sub_path = estrndup(intern->u.dir.entry.d_name, subdir->u.dir.sub_path_len);
		}
		subdir->info_class = intern->info_class;
		subdir->file_class = intern->file_class;
		subdir->oth = intern->oth;
	}
}