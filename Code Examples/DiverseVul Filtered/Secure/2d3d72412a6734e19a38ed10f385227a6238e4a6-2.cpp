PHP_FUNCTION(urldecode)
{
	zend_string *in_str, *out_str;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(in_str)
	ZEND_PARSE_PARAMETERS_END();

	out_str = zend_string_init(ZSTR_VAL(in_str), ZSTR_LEN(in_str), 0);
	ZSTR_LEN(out_str) = php_url_decode(ZSTR_VAL(out_str), ZSTR_LEN(out_str));

    RETURN_NEW_STR(out_str);
}