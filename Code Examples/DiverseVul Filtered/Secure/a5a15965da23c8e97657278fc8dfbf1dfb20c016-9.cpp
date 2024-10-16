SPL_METHOD(DirectoryIterator, seek)
{
	spl_filesystem_object *intern    = Z_SPLFILESYSTEM_P(getThis());
	zval retval;
	zend_long pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &pos) == FAILURE) {
		return;
	}

	if (intern->u.dir.index > pos) {
		/* we first rewind */
		zend_call_method_with_0_params(&EX(This), Z_OBJCE(EX(This)), &intern->u.dir.func_rewind, "rewind", NULL);
	}

	while (intern->u.dir.index < pos) {
		int valid = 0;
		zend_call_method_with_0_params(&EX(This), Z_OBJCE(EX(This)), &intern->u.dir.func_valid, "valid", &retval);
		if (!Z_ISUNDEF(retval)) {
			valid = zend_is_true(&retval);
			zval_ptr_dtor(&retval);
		}
		if (!valid) {
			zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0, "Seek position " ZEND_LONG_FMT " is out of range", pos);
			return;
		}
		zend_call_method_with_0_params(&EX(This), Z_OBJCE(EX(This)), &intern->u.dir.func_next, "next", NULL);
	}
} /* }}} */