PHP_METHOD(Phar, decompress)
{
	char *ext = NULL;
	int ext_len = 0;
	zval *ret;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &ext, &ext_len) == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot decompress phar archive, phar is read-only");
		return;
	}

	if (phar_obj->arc.archive->is_zip) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot decompress zip-based archives with whole-archive compression");
		return;
	}

	if (phar_obj->arc.archive->is_tar) {
		ret = phar_convert_to_other(phar_obj->arc.archive, PHAR_FORMAT_TAR, ext, PHAR_FILE_COMPRESSED_NONE TSRMLS_CC);
	} else {
		ret = phar_convert_to_other(phar_obj->arc.archive, PHAR_FORMAT_PHAR, ext, PHAR_FILE_COMPRESSED_NONE TSRMLS_CC);
	}

	if (ret) {
		RETURN_ZVAL(ret, 1, 1);
	} else {
		RETURN_NULL();
	}
}