static int spl_filesystem_file_read_csv(spl_filesystem_object *intern, char delimiter, char enclosure, char escape, zval *return_value) /* {{{ */
{
	int ret = SUCCESS;
	zval *value;

	do {
		ret = spl_filesystem_file_read(intern, 1);
	} while (ret == SUCCESS && !intern->u.file.current_line_len && SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_SKIP_EMPTY));

	if (ret == SUCCESS) {
		size_t buf_len = intern->u.file.current_line_len;
		char *buf = estrndup(intern->u.file.current_line, buf_len);

		if (!Z_ISUNDEF(intern->u.file.current_zval)) {
			zval_ptr_dtor(&intern->u.file.current_zval);
			ZVAL_UNDEF(&intern->u.file.current_zval);
		}

		php_fgetcsv(intern->u.file.stream, delimiter, enclosure, escape, buf_len, buf, &intern->u.file.current_zval);
		if (return_value) {
			zval_ptr_dtor(return_value);
			value = &intern->u.file.current_zval;
			ZVAL_DEREF(value);
			ZVAL_COPY(return_value, value);
		}
	}
	return ret;
}