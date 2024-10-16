PHP_METHOD(Phar, createDefaultStub)
{
	char *index = NULL, *webindex = NULL, *stub, *error;
	int index_len = 0, webindex_len = 0;
	size_t stub_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|pp", &index, &index_len, &webindex, &webindex_len) == FAILURE) {
		return;
	}

	stub = phar_create_default_stub(index, webindex, &stub_len, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
		return;
	}
	RETURN_STRINGL(stub, stub_len, 0);
}