static char *spl_filesystem_object_get_pathname(spl_filesystem_object *intern, size_t *len) { /* {{{ */
	switch (intern->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			*len = intern->file_name_len;
			return intern->file_name;
		case SPL_FS_DIR:
			if (intern->u.dir.entry.d_name[0]) {
				spl_filesystem_object_get_file_name(intern);
				*len = intern->file_name_len;
				return intern->file_name;
			}
	}
	*len = 0;
	return NULL;
}