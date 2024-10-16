PHP_FUNCTION(urlencode)
{
	zend_string *in_str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(in_str)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_STR(php_url_encode(ZSTR_VAL(in_str), ZSTR_LEN(in_str)));
}