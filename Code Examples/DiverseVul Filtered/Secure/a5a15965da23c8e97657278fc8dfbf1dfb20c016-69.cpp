SPL_METHOD(FilesystemIterator, rewind)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());
	int skip_dots = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_SKIPDOTS);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern->u.dir.index = 0;
	if (intern->u.dir.dirp) {
		php_stream_rewinddir(intern->u.dir.dirp);
	}
	do {
		spl_filesystem_dir_read(intern);
	} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
}