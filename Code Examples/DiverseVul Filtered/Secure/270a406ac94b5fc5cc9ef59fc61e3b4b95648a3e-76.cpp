PHP_FUNCTION(openssl_error_string)
{
	char buf[512];
	unsigned long val;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	val = ERR_get_error();
	if (val) {
		RETURN_STRING(ERR_error_string(val, buf), 1);
	} else {
		RETURN_FALSE;
	}
}