PHP_METHOD(Phar, compress)
{
	long method;
	char *ext = NULL;
	int ext_len = 0;
	php_uint32 flags;
	zval *ret;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|s", &method, &ext, &ext_len) == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot compress phar archive, phar is read-only");
		return;
	}

	if (phar_obj->arc.archive->is_zip) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot compress zip-based archives with whole-archive compression");
		return;
	}

	switch (method) {
		case 0:
			flags = PHAR_FILE_COMPRESSED_NONE;
			break;
		case PHAR_ENT_COMPRESSED_GZ:
			if (!PHAR_G(has_zlib)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with gzip, enable ext/zlib in php.ini");
				return;
			}
			flags = PHAR_FILE_COMPRESSED_GZ;
			break;

		case PHAR_ENT_COMPRESSED_BZ2:
			if (!PHAR_G(has_bz2)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with bz2, enable ext/bz2 in php.ini");
				return;
			}
			flags = PHAR_FILE_COMPRESSED_BZ2;
			break;
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
				"Unknown compression specified, please pass one of Phar::GZ or Phar::BZ2");
			return;
	}

	if (phar_obj->arc.archive->is_tar) {
		ret = phar_convert_to_other(phar_obj->arc.archive, PHAR_FORMAT_TAR, ext, flags TSRMLS_CC);
	} else {
		ret = phar_convert_to_other(phar_obj->arc.archive, PHAR_FORMAT_PHAR, ext, flags TSRMLS_CC);
	}

	if (ret) {
		RETURN_ZVAL(ret, 1, 1);
	} else {
		RETURN_NULL();
	}
}