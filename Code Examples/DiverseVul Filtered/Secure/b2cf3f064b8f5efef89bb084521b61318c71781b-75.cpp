PHP_METHOD(Phar, convertToExecutable)
{
	char *ext = NULL;
	int is_data, ext_len = 0;
	php_uint32 flags;
	zval *ret;
	/* a number that is not 0, 1 or 2 (Which is also Greg's birthday, so there) */
	long format = 9021976, method = 9021976;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lls", &format, &method, &ext, &ext_len) == FAILURE) {
		return;
	}

	if (PHAR_G(readonly)) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write out executable phar archive, phar is read-only");
		return;
	}

	switch (format) {
		case 9021976:
		case PHAR_FORMAT_SAME: /* null is converted to 0 */
			/* by default, use the existing format */
			if (phar_obj->arc.archive->is_tar) {
				format = PHAR_FORMAT_TAR;
			} else if (phar_obj->arc.archive->is_zip) {
				format = PHAR_FORMAT_ZIP;
			} else {
				format = PHAR_FORMAT_PHAR;
			}
			break;
		case PHAR_FORMAT_PHAR:
		case PHAR_FORMAT_TAR:
		case PHAR_FORMAT_ZIP:
			break;
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
				"Unknown file format specified, please pass one of Phar::PHAR, Phar::TAR or Phar::ZIP");
			return;
	}

	switch (method) {
		case 9021976:
			flags = phar_obj->arc.archive->flags & PHAR_FILE_COMPRESSION_MASK;
			break;
		case 0:
			flags = PHAR_FILE_COMPRESSED_NONE;
			break;
		case PHAR_ENT_COMPRESSED_GZ:
			if (format == PHAR_FORMAT_ZIP) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with gzip, zip archives do not support whole-archive compression");
				return;
			}

			if (!PHAR_G(has_zlib)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with gzip, enable ext/zlib in php.ini");
				return;
			}

			flags = PHAR_FILE_COMPRESSED_GZ;
			break;
		case PHAR_ENT_COMPRESSED_BZ2:
			if (format == PHAR_FORMAT_ZIP) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress entire archive with bz2, zip archives do not support whole-archive compression");
				return;
			}

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

	is_data = phar_obj->arc.archive->is_data;
	phar_obj->arc.archive->is_data = 0;
	ret = phar_convert_to_other(phar_obj->arc.archive, format, ext, flags TSRMLS_CC);
	phar_obj->arc.archive->is_data = is_data;

	if (ret) {
		RETURN_ZVAL(ret, 1, 1);
	} else {
		RETURN_NULL();
	}
}