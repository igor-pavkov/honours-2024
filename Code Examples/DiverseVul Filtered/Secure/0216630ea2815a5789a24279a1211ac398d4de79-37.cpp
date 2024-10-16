PHP_FUNCTION(openssl_get_cipher_methods)
{
	zend_bool aliases = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &aliases) == FAILURE) {
		return;
	}
	array_init(return_value);
	OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_CIPHER_METH,
		aliases ? php_openssl_add_method_or_alias: php_openssl_add_method,
		return_value);
}