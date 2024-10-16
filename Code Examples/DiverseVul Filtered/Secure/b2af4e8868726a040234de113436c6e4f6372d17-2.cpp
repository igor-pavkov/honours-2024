PHP_FUNCTION(memory_get_usage) {
	zend_bool real_usage = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &real_usage) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_LONG(zend_memory_usage(real_usage));
}