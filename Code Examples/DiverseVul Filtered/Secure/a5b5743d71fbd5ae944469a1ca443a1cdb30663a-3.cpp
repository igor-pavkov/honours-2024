PHP_FUNCTION(filter_has_var)
{
	long        arg;
	char       *var;
	int         var_len;
	zval       *array_ptr = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &arg, &var, &var_len) == FAILURE) {
		RETURN_FALSE;
	}

	array_ptr = php_filter_get_storage(arg TSRMLS_CC);

	if (array_ptr && HASH_OF(array_ptr) && zend_hash_exists(HASH_OF(array_ptr), var, var_len + 1)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}