PHP_FUNCTION(serialize)
{
	zval *struc;
	php_serialize_data_t var_hash;
	smart_str buf = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &struc) == FAILURE) {
		return;
	}

	PHP_VAR_SERIALIZE_INIT(var_hash);
	php_var_serialize(&buf, struc, &var_hash);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	if (EG(exception)) {
		smart_str_free(&buf);
		RETURN_FALSE;
	}

	if (buf.s) {
		RETURN_NEW_STR(buf.s);
	} else {
		RETURN_NULL();
	}
}