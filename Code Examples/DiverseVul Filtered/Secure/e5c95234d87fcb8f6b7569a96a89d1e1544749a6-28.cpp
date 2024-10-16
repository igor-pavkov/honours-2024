PHP_METHOD(Phar, compress)
{
	zend_long method;
	char *ext = NULL;
	size_t ext_len = 0;
	uint32_t flags;
	zend_object *ret;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|s", &method, &ext, &ext_len) == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"Cannot compress phar archive, phar is read-only");
		return;
	}

	if (phar_obj->archive->is_zip) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"Cannot compress zip-based archives with whole-archive compression");
		return;
	}

	switch (method) {
		case 0:
			flags = PHAR_FILE_COMPRESSED_NONE;
			break;
		case PHAR_ENT_COMPRESSED_GZ:
			if (!PHAR_G(has_zlib)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
					"Cannot compress entire archive with gzip, enable ext/zlib in php.ini");
				return;
			}
			flags = PHAR_FILE_COMPRESSED_GZ;
			break;

		case PHAR_ENT_COMPRESSED_BZ2:
			if (!PHAR_G(has_bz2)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
					"Cannot compress entire archive with bz2, enable ext/bz2 in php.ini");
				return;
			}
			flags = PHAR_FILE_COMPRESSED_BZ2;
			break;
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
				"Unknown compression specified, please pass one of Phar::GZ or Phar::BZ2");
			return;
	}

	if (phar_obj->archive->is_tar) {
		ret = phar_convert_to_other(phar_obj->archive, PHAR_FORMAT_TAR, ext, flags);
	} else {
		ret = phar_convert_to_other(phar_obj->archive, PHAR_FORMAT_PHAR, ext, flags);
	}

	if (ret) {
		ZVAL_OBJ(return_value, ret);
	} else {
		RETURN_NULL();
	}
}