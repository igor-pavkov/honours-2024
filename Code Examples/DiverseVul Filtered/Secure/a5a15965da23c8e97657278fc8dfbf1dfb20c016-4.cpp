SPL_METHOD(DirectoryIterator, next)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	int skip_dots = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_SKIPDOTS);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern->u.dir.index++;
	do {
		spl_filesystem_dir_read(intern);
	} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
	if (intern->file_name) {
		efree(intern->file_name);
		intern->file_name = NULL;
	}
}