PHP_METHOD(Phar, addFromString)
{
	char *localname, *cont_str;
	int localname_len, cont_len;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ps", &localname, &localname_len, &cont_str, &cont_len) == FAILURE) {
		return;
	}

	phar_add_file(&(phar_obj->arc.archive), localname, localname_len, cont_str, cont_len, NULL TSRMLS_CC);
}