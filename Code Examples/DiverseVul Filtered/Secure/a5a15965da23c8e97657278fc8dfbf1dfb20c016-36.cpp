static inline void spl_filesystem_object_get_file_name(spl_filesystem_object *intern) /* {{{ */
{
	char slash = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_UNIXPATHS) ? '/' : DEFAULT_SLASH;

	switch (intern->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			if (!intern->file_name) {
				php_error_docref(NULL, E_ERROR, "Object not initialized");
			}
			break;
		case SPL_FS_DIR:
			{
				size_t path_len = 0;
				char *path = spl_filesystem_object_get_path(intern, &path_len);
				if (intern->file_name) {
					efree(intern->file_name);
				}
				/* if there is parent path, ammend it, otherwise just use the given path as is */
				if (path_len == 0) {
					intern->file_name_len = spprintf(
						&intern->file_name, 0, "%s", intern->u.dir.entry.d_name);
				} else {
					intern->file_name_len = spprintf(
						&intern->file_name, 0, "%s%c%s", path, slash, intern->u.dir.entry.d_name);
				}
			}
			break;
	}
} /* }}} */