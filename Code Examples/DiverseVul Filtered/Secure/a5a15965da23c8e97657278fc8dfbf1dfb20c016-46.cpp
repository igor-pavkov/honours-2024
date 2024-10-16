PHPAPI char* spl_filesystem_object_get_path(spl_filesystem_object *intern, size_t *len) /* {{{ */
{
#ifdef HAVE_GLOB
	if (intern->type == SPL_FS_DIR) {
		if (php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
			return php_glob_stream_get_path(intern->u.dir.dirp, 0, len);
		}
	}
#endif
	if (len) {
		*len = intern->_path_len;
	}
	return intern->_path;
} /* }}} */