PHP_METHOD(Phar, interceptFileFuncs)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	phar_intercept_functions(TSRMLS_C);
}