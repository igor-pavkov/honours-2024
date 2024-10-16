static zval *php_filter_get_storage(long arg TSRMLS_DC)/* {{{ */

{
	zval *array_ptr = NULL;
	zend_bool jit_initialization = (PG(auto_globals_jit) && !PG(register_globals) && !PG(register_long_arrays));

	switch (arg) {
		case PARSE_GET:
			array_ptr = IF_G(get_array);
			break;
		case PARSE_POST:
			array_ptr = IF_G(post_array);
			break;
		case PARSE_COOKIE:
			array_ptr = IF_G(cookie_array);
			break;
		case PARSE_SERVER:
			if (jit_initialization) {
				zend_is_auto_global("_SERVER", sizeof("_SERVER")-1 TSRMLS_CC);
			}
			array_ptr = IF_G(server_array);
			break;
		case PARSE_ENV:
			if (jit_initialization) {
				zend_is_auto_global("_ENV", sizeof("_ENV")-1 TSRMLS_CC);
			}
			array_ptr = IF_G(env_array);
			break;
		case PARSE_SESSION:
			/* FIXME: Implement session source */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "INPUT_SESSION is not yet implemented");
			break;
		case PARSE_REQUEST:
			/* FIXME: Implement request source */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "INPUT_REQUEST is not yet implemented");
			break;
	}

	return array_ptr;
}