static int spl_filesystem_file_call(spl_filesystem_object *intern, zend_function *func_ptr, int pass_num_args, zval *return_value, zval *arg2) /* {{{ */
{
	zend_fcall_info fci;
	zend_fcall_info_cache fcic;
	zval *zresource_ptr = &intern->u.file.zresource, retval;
	int result;
	int num_args = pass_num_args + (arg2 ? 2 : 1);

	zval *params = (zval*)safe_emalloc(num_args, sizeof(zval), 0);

	params[0] = *zresource_ptr;

	if (arg2) {
		params[1] = *arg2;
	}

	if (zend_get_parameters_array_ex(pass_num_args, params + (arg2 ? 2 : 1)) != SUCCESS) {
		efree(params);
		WRONG_PARAM_COUNT_WITH_RETVAL(FAILURE);
	}

	ZVAL_UNDEF(&retval);

	fci.size = sizeof(fci);
	fci.object = NULL;
	fci.retval = &retval;
	fci.param_count = num_args;
	fci.params = params;
	fci.no_separation = 1;
	ZVAL_STR(&fci.function_name, func_ptr->common.function_name);

	fcic.initialized = 1;
	fcic.function_handler = func_ptr;
	fcic.calling_scope = NULL;
	fcic.called_scope = NULL;
	fcic.object = NULL;

	result = zend_call_function(&fci, &fcic);

	if (result == FAILURE || Z_ISUNDEF(retval)) {
		RETVAL_FALSE;
	} else {
		ZVAL_ZVAL(return_value, &retval, 0, 0);
	}

	efree(params);
	return result;
} /* }}} */