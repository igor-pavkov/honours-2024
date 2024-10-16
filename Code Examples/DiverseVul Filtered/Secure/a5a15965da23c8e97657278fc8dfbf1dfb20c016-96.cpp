static void spl_filesystem_object_free_storage(zend_object *object) /* {{{ */
{
	spl_filesystem_object *intern = spl_filesystem_from_obj(object);

	if (intern->oth_handler && intern->oth_handler->dtor) {
		intern->oth_handler->dtor(intern);
	}

	zend_object_std_dtor(&intern->std);

	if (intern->_path) {
		efree(intern->_path);
	}
	if (intern->file_name) {
		efree(intern->file_name);
	}
	switch(intern->type) {
	case SPL_FS_INFO:
		break;
	case SPL_FS_DIR:
		if (intern->u.dir.sub_path) {
			efree(intern->u.dir.sub_path);
		}
		break;
	case SPL_FS_FILE:
		if (intern->u.file.open_mode) {
			efree(intern->u.file.open_mode);
		}
		if (intern->orig_path) {
			efree(intern->orig_path);
		}
		spl_filesystem_file_free_line(intern);
		break;
	}
} /* }}} */