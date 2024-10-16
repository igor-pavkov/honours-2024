PHP_METHOD(Phar, setMetadata)
{
	char *error;
	zval *metadata;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &metadata) == FAILURE) {
		return;
	}

	if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
		return;
	}
	if (phar_obj->arc.archive->metadata) {
		zval_ptr_dtor(&phar_obj->arc.archive->metadata);
		phar_obj->arc.archive->metadata = NULL;
	}

	MAKE_STD_ZVAL(phar_obj->arc.archive->metadata);
	ZVAL_ZVAL(phar_obj->arc.archive->metadata, metadata, 1, 0);
	phar_obj->arc.archive->is_modified = 1;
	phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
}