PHP_METHOD(Phar, canWrite)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_BOOL(!PHAR_G(readonly));
}