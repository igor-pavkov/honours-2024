static int spl_filesystem_file_read_line_ex(zval * this_ptr, spl_filesystem_object *intern, int silent) /* {{{ */
{
	zval retval;

	/* 1) use fgetcsv? 2) overloaded call the function, 3) do it directly */
	if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV) || intern->u.file.func_getCurr->common.scope != spl_ce_SplFileObject) {
		if (php_stream_eof(intern->u.file.stream)) {
			if (!silent) {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Cannot read from file %s", intern->file_name);
			}
			return FAILURE;
		}
		if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV)) {
			return spl_filesystem_file_read_csv(intern, intern->u.file.delimiter, intern->u.file.enclosure, intern->u.file.escape, NULL);
		} else {
			zend_execute_data *execute_data = EG(current_execute_data);
			zend_call_method_with_0_params(this_ptr, Z_OBJCE(EX(This)), &intern->u.file.func_getCurr, "getCurrentLine", &retval);
		}
		if (!Z_ISUNDEF(retval)) {
			if (intern->u.file.current_line || !Z_ISUNDEF(intern->u.file.current_zval)) {
				intern->u.file.current_line_num++;
			}
			spl_filesystem_file_free_line(intern);
			if (Z_TYPE(retval) == IS_STRING) {
				intern->u.file.current_line = estrndup(Z_STRVAL(retval), Z_STRLEN(retval));
				intern->u.file.current_line_len = Z_STRLEN(retval);
			} else {
				zval *value = &retval;

				ZVAL_DEREF(value);
				ZVAL_COPY(&intern->u.file.current_zval, value);
			}
			zval_ptr_dtor(&retval);
			return SUCCESS;
		} else {
			return FAILURE;
		}
	} else {
		return spl_filesystem_file_read(intern, silent);
	}
} /* }}} */