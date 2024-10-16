static void spl_filesystem_file_free_line(spl_filesystem_object *intern) /* {{{ */
{
	if (intern->u.file.current_line) {
		efree(intern->u.file.current_line);
		intern->u.file.current_line = NULL;
	}
	if (!Z_ISUNDEF(intern->u.file.current_zval)) {
		zval_ptr_dtor(&intern->u.file.current_zval);
		ZVAL_UNDEF(&intern->u.file.current_zval);
	}
} /* }}} */