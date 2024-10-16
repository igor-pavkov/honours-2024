SPL_METHOD(RecursiveDirectoryIterator, hasChildren)
{
	zend_bool allow_links = 0;
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(getThis());

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &allow_links) == FAILURE) {
		return;
	}
	if (spl_filesystem_is_invalid_or_dot(intern->u.dir.entry.d_name)) {
		RETURN_FALSE;
	} else {
		spl_filesystem_object_get_file_name(intern);
		if (!allow_links && !(intern->flags & SPL_FILE_DIR_FOLLOW_SYMLINKS)) {
			php_stat(intern->file_name, intern->file_name_len, FS_IS_LINK, return_value);
			if (zend_is_true(return_value)) {
				RETURN_FALSE;
			}
		}
		php_stat(intern->file_name, intern->file_name_len, FS_IS_DIR, return_value);
    }
}