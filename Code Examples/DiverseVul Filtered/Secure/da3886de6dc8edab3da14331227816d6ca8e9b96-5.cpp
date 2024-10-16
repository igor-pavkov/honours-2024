PHP_MINIT_FUNCTION(filter)
{
	ZEND_INIT_MODULE_GLOBALS(filter, php_filter_init_globals, NULL);

	REGISTER_INI_ENTRIES();

	REGISTER_LONG_CONSTANT("INPUT_POST",	PARSE_POST, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_GET",		PARSE_GET,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_COOKIE",	PARSE_COOKIE, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_ENV",		PARSE_ENV,		CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_SERVER",	PARSE_SERVER, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_SESSION", PARSE_SESSION, 	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INPUT_REQUEST", PARSE_REQUEST, 	CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_NONE", FILTER_FLAG_NONE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_REQUIRE_SCALAR", FILTER_REQUIRE_SCALAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_REQUIRE_ARRAY", FILTER_REQUIRE_ARRAY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FORCE_ARRAY", FILTER_FORCE_ARRAY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_NULL_ON_FAILURE", FILTER_NULL_ON_FAILURE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_INT", FILTER_VALIDATE_INT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_BOOLEAN", FILTER_VALIDATE_BOOLEAN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_FLOAT", FILTER_VALIDATE_FLOAT, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_REGEXP", FILTER_VALIDATE_REGEXP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_URL", FILTER_VALIDATE_URL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_EMAIL", FILTER_VALIDATE_EMAIL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_VALIDATE_IP", FILTER_VALIDATE_IP, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_DEFAULT", FILTER_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_UNSAFE_RAW", FILTER_UNSAFE_RAW, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_STRING", FILTER_SANITIZE_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_STRIPPED", FILTER_SANITIZE_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_ENCODED", FILTER_SANITIZE_ENCODED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_SPECIAL_CHARS", FILTER_SANITIZE_SPECIAL_CHARS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_FULL_SPECIAL_CHARS", FILTER_SANITIZE_FULL_SPECIAL_CHARS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_EMAIL", FILTER_SANITIZE_EMAIL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_URL", FILTER_SANITIZE_URL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_NUMBER_INT", FILTER_SANITIZE_NUMBER_INT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_NUMBER_FLOAT", FILTER_SANITIZE_NUMBER_FLOAT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_SANITIZE_MAGIC_QUOTES", FILTER_SANITIZE_MAGIC_QUOTES, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_CALLBACK", FILTER_CALLBACK, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_ALLOW_OCTAL", FILTER_FLAG_ALLOW_OCTAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ALLOW_HEX", FILTER_FLAG_ALLOW_HEX, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_STRIP_LOW", FILTER_FLAG_STRIP_LOW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_STRIP_HIGH", FILTER_FLAG_STRIP_HIGH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_STRIP_BACKTICK", FILTER_FLAG_STRIP_BACKTICK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ENCODE_LOW", FILTER_FLAG_ENCODE_LOW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ENCODE_HIGH", FILTER_FLAG_ENCODE_HIGH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ENCODE_AMP", FILTER_FLAG_ENCODE_AMP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_NO_ENCODE_QUOTES", FILTER_FLAG_NO_ENCODE_QUOTES, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_EMPTY_STRING_NULL", FILTER_FLAG_EMPTY_STRING_NULL, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_ALLOW_FRACTION", FILTER_FLAG_ALLOW_FRACTION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ALLOW_THOUSAND", FILTER_FLAG_ALLOW_THOUSAND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_ALLOW_SCIENTIFIC", FILTER_FLAG_ALLOW_SCIENTIFIC, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_SCHEME_REQUIRED", FILTER_FLAG_SCHEME_REQUIRED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_HOST_REQUIRED", FILTER_FLAG_HOST_REQUIRED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_PATH_REQUIRED", FILTER_FLAG_PATH_REQUIRED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_QUERY_REQUIRED", FILTER_FLAG_QUERY_REQUIRED, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("FILTER_FLAG_IPV4", FILTER_FLAG_IPV4, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_IPV6", FILTER_FLAG_IPV6, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_NO_RES_RANGE", FILTER_FLAG_NO_RES_RANGE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("FILTER_FLAG_NO_PRIV_RANGE", FILTER_FLAG_NO_PRIV_RANGE, CONST_CS | CONST_PERSISTENT);

	sapi_register_input_filter(php_sapi_filter, php_sapi_filter_init TSRMLS_CC);

	return SUCCESS;
}