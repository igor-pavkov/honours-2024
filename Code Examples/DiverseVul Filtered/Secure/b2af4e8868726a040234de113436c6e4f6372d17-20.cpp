PHP_FUNCTION(var_export)
{
	zval *var;
	zend_bool return_output = 0;
	smart_str buf = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &var, &return_output) == FAILURE) {
		return;
	}

	php_var_export_ex(var, 1, &buf);
	smart_str_0 (&buf);

	if (return_output) {
		RETURN_NEW_STR(buf.s);
	} else {
		PHPWRITE(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
		smart_str_free(&buf);
	}
}