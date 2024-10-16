PHP_METHOD(Phar, offsetSet)
{
	char *fname, *cont_str = NULL;
	int fname_len, cont_len;
	zval *zresource;
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "pr", &fname, &fname_len, &zresource) == FAILURE
	&& zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ps", &fname, &fname_len, &cont_str, &cont_len) == FAILURE) {
		return;
	}

	if (fname_len == sizeof(".phar/stub.php")-1 && !memcmp(fname, ".phar/stub.php", sizeof(".phar/stub.php")-1)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot set stub \".phar/stub.php\" directly in phar \"%s\", use setStub", phar_obj->arc.archive->fname);
		return;
	}

	if (fname_len == sizeof(".phar/alias.txt")-1 && !memcmp(fname, ".phar/alias.txt", sizeof(".phar/alias.txt")-1)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot set alias \".phar/alias.txt\" directly in phar \"%s\", use setAlias", phar_obj->arc.archive->fname);
		return;
	}

	if (fname_len >= sizeof(".phar")-1 && !memcmp(fname, ".phar", sizeof(".phar")-1)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Cannot set any files or directories in magic \".phar\" directory", phar_obj->arc.archive->fname);
		return;
	}

	phar_add_file(&(phar_obj->arc.archive), fname, fname_len, cont_str, cont_len, zresource TSRMLS_CC);
}