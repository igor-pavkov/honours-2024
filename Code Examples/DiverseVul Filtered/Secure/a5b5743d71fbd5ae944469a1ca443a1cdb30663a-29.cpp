PHP_FUNCTION(filter_var_array)
{
	zval *array_input = NULL, **op = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a|Z",  &array_input, &op) == FAILURE) {
		return;
	}

	if (op
		&& (Z_TYPE_PP(op) != IS_ARRAY)
		&& (Z_TYPE_PP(op) == IS_LONG && !PHP_FILTER_ID_EXISTS(Z_LVAL_PP(op)))
		) {
		RETURN_FALSE;
	}

	php_filter_array_handler(array_input, op, return_value TSRMLS_CC);
}