PHP_METHOD(Phar, buildFromIterator)
{
	zval *obj;
	char *error;
	uint base_len = 0;
	char *base = NULL;
	struct _phar_t pass;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write out phar archive, phar is read-only");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|s", &obj, zend_ce_traversable, &base, &base_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
		return;
	}

	array_init(return_value);

	pass.c = Z_OBJCE_P(obj);
	pass.p = phar_obj;
	pass.b = base;
	pass.l = base_len;
	pass.ret = return_value;
	pass.count = 0;
	pass.fp = php_stream_fopen_tmpfile();
	if (pass.fp == NULL) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\": unable to create temporary file", phar_obj->arc.archive->fname);
		return;
	}

	if (SUCCESS == spl_iterator_apply(obj, (spl_iterator_apply_func_t) phar_build, (void *) &pass TSRMLS_CC)) {
		phar_obj->arc.archive->ufp = pass.fp;
		phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);
		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
		}
	} else {
		php_stream_close(pass.fp);
	}
}