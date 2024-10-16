PHP_FUNCTION(filter_var)
{
	long filter = FILTER_DEFAULT;
	zval **filter_args = NULL, *data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/|lZ", &data, &filter, &filter_args) == FAILURE) {
		return;
	}

	if (!PHP_FILTER_ID_EXISTS(filter)) {
		RETURN_FALSE;
	}

	MAKE_COPY_ZVAL(&data, return_value);

	php_filter_call(&return_value, filter, filter_args, 1, FILTER_REQUIRE_SCALAR TSRMLS_CC);
}