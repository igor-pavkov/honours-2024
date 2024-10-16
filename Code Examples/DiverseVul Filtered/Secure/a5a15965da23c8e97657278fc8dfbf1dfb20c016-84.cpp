static int spl_filesystem_object_cast(zval *readobj, zval *writeobj, int type)
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(readobj);

	if (type == IS_STRING) {
		if (Z_OBJCE_P(readobj)->__tostring) {
			return std_object_handlers.cast_object(readobj, writeobj, type);
		}

		switch (intern->type) {
		case SPL_FS_INFO:
		case SPL_FS_FILE:
			if (readobj == writeobj) {
				zval retval;
				zval *retval_ptr = &retval;

				ZVAL_STRINGL(retval_ptr, intern->file_name, intern->file_name_len);
				zval_ptr_dtor(readobj);
				ZVAL_NEW_STR(writeobj, Z_STR_P(retval_ptr));
			} else {
				ZVAL_STRINGL(writeobj, intern->file_name, intern->file_name_len);
			}
			return SUCCESS;
		case SPL_FS_DIR:
			if (readobj == writeobj) {
				zval retval;
				zval *retval_ptr = &retval;

				ZVAL_STRING(retval_ptr, intern->u.dir.entry.d_name);
				zval_ptr_dtor(readobj);
				ZVAL_NEW_STR(writeobj, Z_STR_P(retval_ptr));
			} else {
				ZVAL_STRING(writeobj, intern->u.dir.entry.d_name);
			}
			return SUCCESS;
		}
	} else if (type == _IS_BOOL) {
		ZVAL_TRUE(writeobj);
		return SUCCESS;
	}
	if (readobj == writeobj) {
		zval_ptr_dtor(readobj);
	}
	ZVAL_NULL(writeobj);
	return FAILURE;
}