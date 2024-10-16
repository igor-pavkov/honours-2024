static int spl_filesystem_file_is_empty_line(spl_filesystem_object *intern) /* {{{ */
{
	if (intern->u.file.current_line) {
		return intern->u.file.current_line_len == 0;
	} else if (!Z_ISUNDEF(intern->u.file.current_zval)) {
		switch(Z_TYPE(intern->u.file.current_zval)) {
			case IS_STRING:
				return Z_STRLEN(intern->u.file.current_zval) == 0;
			case IS_ARRAY:
				if (SPL_HAS_FLAG(intern->flags, SPL_FILE_OBJECT_READ_CSV)
						&& zend_hash_num_elements(Z_ARRVAL(intern->u.file.current_zval)) == 1) {
					uint32_t idx = 0;
					zval *first;

					while (Z_ISUNDEF(Z_ARRVAL(intern->u.file.current_zval)->arData[idx].val)) {
						idx++;
					}
					first = &Z_ARRVAL(intern->u.file.current_zval)->arData[idx].val;
					return Z_TYPE_P(first) == IS_STRING && Z_STRLEN_P(first) == 0;
				}
				return zend_hash_num_elements(Z_ARRVAL(intern->u.file.current_zval)) == 0;
			case IS_NULL:
				return 1;
			default:
				return 0;
		}
	} else {
		return 1;
	}
}