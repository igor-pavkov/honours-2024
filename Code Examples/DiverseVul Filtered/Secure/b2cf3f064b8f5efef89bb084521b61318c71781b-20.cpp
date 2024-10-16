PHP_METHOD(Phar, apiVersion)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_STRINGL(PHP_PHAR_API_VERSION, sizeof(PHP_PHAR_API_VERSION)-1, 1);
}