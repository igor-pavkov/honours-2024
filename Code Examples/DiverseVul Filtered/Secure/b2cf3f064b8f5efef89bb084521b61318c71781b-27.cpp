PHP_METHOD(Phar, setDefaultStub)
{
	char *index = NULL, *webindex = NULL, *error = NULL, *stub = NULL;
	int index_len = 0, webindex_len = 0, created_stub = 0;
	size_t stub_len = 0;
	PHAR_ARCHIVE_OBJECT();

	if (phar_obj->arc.archive->is_data) {
		if (phar_obj->arc.archive->is_tar) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"A Phar stub cannot be set in a plain tar archive");
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"A Phar stub cannot be set in a plain zip archive");
		}
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!s", &index, &index_len, &webindex, &webindex_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() > 0 && (phar_obj->arc.archive->is_tar || phar_obj->arc.archive->is_zip)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "method accepts no arguments for a tar- or zip-based phar stub, %d given", ZEND_NUM_ARGS());
		RETURN_FALSE;
	}

	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot change stub: phar.readonly=1");
		RETURN_FALSE;
	}

	if (!phar_obj->arc.archive->is_tar && !phar_obj->arc.archive->is_zip) {
		stub = phar_create_default_stub(index, webindex, &stub_len, &error TSRMLS_CC);

		if (error) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "%s", error);
			efree(error);
			if (stub) {
				efree(stub);
			}
			RETURN_FALSE;
		}

		created_stub = 1;
	}

	if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
		return;
	}
	phar_flush(phar_obj->arc.archive, stub, stub_len, 1, &error TSRMLS_CC);

	if (created_stub) {
		efree(stub);
	}

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}