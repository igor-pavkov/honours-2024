static int spl_filesystem_file_read_line(zval * this_ptr, spl_filesystem_object *intern, int silent) /* {{{ */
{
	int ret = spl_filesystem_file_read_line_ex(this_ptr, intern, silent);

	while (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_SKIP_EMPTY) && ret == SUCCESS && spl_filesystem_file_is_empty_line(intern)) {
		spl_filesystem_file_free_line(intern);
		ret = spl_filesystem_file_read_line_ex(this_ptr, intern, silent);
	}

	return ret;
}