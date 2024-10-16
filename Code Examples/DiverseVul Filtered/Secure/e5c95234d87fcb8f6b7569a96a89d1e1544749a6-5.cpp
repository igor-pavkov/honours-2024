PHP_METHOD(Phar, addEmptyDir)
{
	char *dirname;
	size_t dirname_len;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &dirname, &dirname_len) == FAILURE) {
		return;
	}
	if (ZEND_SIZE_T_INT_OVFL(dirname_len)) {
		RETURN_FALSE;
	}

	if (dirname_len >= sizeof(".phar")-1 && !memcmp(dirname, ".phar", sizeof(".phar")-1)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Cannot create a directory in magic \".phar\" directory");
		return;
	}

	phar_mkdir(&phar_obj->archive, dirname, (int)dirname_len);
}