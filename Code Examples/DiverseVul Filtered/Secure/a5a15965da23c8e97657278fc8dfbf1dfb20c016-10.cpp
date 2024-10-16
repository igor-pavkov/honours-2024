static void spl_filesystem_object_destroy_object(zend_object *object) /* {{{ */
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(object);

	zend_objects_destroy_object(object);

	switch(intern->type) {
	case SPL_FS_DIR:
		if (intern->u.dir.dirp) {
			php_stream_close(intern->u.dir.dirp);
			intern->u.dir.dirp = NULL;
		}
		break;
	case SPL_FS_FILE:
		if (intern->u.file.stream) {
			/*
			if (intern->u.file.zcontext) {
			   zend_list_delref(Z_RESVAL_P(intern->zcontext));
			}
			*/
			if (!intern->u.file.stream->is_persistent) {
				php_stream_close(intern->u.file.stream);
			} else {
				php_stream_pclose(intern->u.file.stream);
			}
			intern->u.file.stream = NULL;
		}
		break;
	}
} /* }}} */