static spl_filesystem_object *spl_filesystem_object_create_type(int ht, spl_filesystem_object *source, int type, zend_class_entry *ce, zval *return_value) /* {{{ */
{
	spl_filesystem_object *intern;
	zend_bool use_include_path = 0;
	zval arg1, arg2;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling);

	switch (source->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			break;
		case SPL_FS_DIR:
			if (!source->u.dir.entry.d_name[0]) {
				zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Could not open file");
				zend_restore_error_handling(&error_handling);
				return NULL;
			}
	}

	switch (type) {
		case SPL_FS_INFO:
			ce = ce ? ce : source->info_class;

			if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
				break;
			}

			intern = spl_filesystem_from_obj(spl_filesystem_object_new_ex(ce));
			ZVAL_OBJ(return_value, &intern->std);

			spl_filesystem_object_get_file_name(source);
			if (ce->constructor->common.scope != spl_ce_SplFileInfo) {
				ZVAL_STRINGL(&arg1, source->file_name, source->file_name_len);
				zend_call_method_with_1_params(return_value, ce, &ce->constructor, "__construct", NULL, &arg1);
				zval_ptr_dtor(&arg1);
			} else {
				intern->file_name = estrndup(source->file_name, source->file_name_len);
				intern->file_name_len = source->file_name_len;
				intern->_path = spl_filesystem_object_get_path(source, &intern->_path_len);
				intern->_path = estrndup(intern->_path, intern->_path_len);
			}
			break;
		case SPL_FS_FILE:
			ce = ce ? ce : source->file_class;

			if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
				break;
			}

			intern = spl_filesystem_from_obj(spl_filesystem_object_new_ex(ce));

			ZVAL_OBJ(return_value, &intern->std);

			spl_filesystem_object_get_file_name(source);

			if (ce->constructor->common.scope != spl_ce_SplFileObject) {
				ZVAL_STRINGL(&arg1, source->file_name, source->file_name_len);
				ZVAL_STRINGL(&arg2, "r", 1);
				zend_call_method_with_2_params(return_value, ce, &ce->constructor, "__construct", NULL, &arg1, &arg2);
				zval_ptr_dtor(&arg1);
				zval_ptr_dtor(&arg2);
			} else {
				intern->file_name = source->file_name;
				intern->file_name_len = source->file_name_len;
				intern->_path = spl_filesystem_object_get_path(source, &intern->_path_len);
				intern->_path = estrndup(intern->_path, intern->_path_len);

				intern->u.file.open_mode = "r";
				intern->u.file.open_mode_len = 1;

				if (ht && zend_parse_parameters(ht, "|sbr",
							&intern->u.file.open_mode, &intern->u.file.open_mode_len,
							&use_include_path, &intern->u.file.zcontext) == FAILURE) {
					zend_restore_error_handling(&error_handling);
					intern->u.file.open_mode = NULL;
					intern->file_name = NULL;
					zval_ptr_dtor(return_value);
					ZVAL_NULL(return_value);
					return NULL;
				}

				if (spl_filesystem_file_open(intern, use_include_path, 0) == FAILURE) {
					zend_restore_error_handling(&error_handling);
					zval_ptr_dtor(return_value);
					ZVAL_NULL(return_value);
					return NULL;
				}
			}
			break;
		case SPL_FS_DIR:
			zend_restore_error_handling(&error_handling);
			zend_throw_exception_ex(spl_ce_RuntimeException, 0, "Operation not supported");
			return NULL;
	}
	zend_restore_error_handling(&error_handling);
	return NULL;
} /* }}} */