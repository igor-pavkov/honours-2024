PHP_METHOD(Phar, addFromString)
{
	char *localname, *cont_str;
	size_t localname_len, cont_len;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ps", &localname, &localname_len, &cont_str, &cont_len) == FAILURE) {
		return;
	}
	if (ZEND_SIZE_T_INT_OVFL(localname_len)) {
		RETURN_FALSE;
	}

	phar_add_file(&(phar_obj->archive), localname, (int)localname_len, cont_str, cont_len, NULL);
}