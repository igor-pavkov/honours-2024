PHP_FUNCTION(debug_zval_dump)
{
	zval *args;
	int argc;
	int	i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &args, &argc) == FAILURE) {
		return;
	}

	for (i = 0; i < argc; i++) {
		php_debug_zval_dump(&args[i], 1);
	}
}