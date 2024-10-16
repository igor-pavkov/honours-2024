PHP_METHOD(Phar, compressFiles)
{
	char *error;
	uint32_t flags;
	zend_long method;
	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &method) == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
			"Phar is readonly, cannot change compression");
		return;
	}

	switch (method) {
		case PHAR_ENT_COMPRESSED_GZ:
			if (!PHAR_G(has_zlib)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
					"Cannot compress files within archive with gzip, enable ext/zlib in php.ini");
				return;
			}
			flags = PHAR_ENT_COMPRESSED_GZ;
			break;

		case PHAR_ENT_COMPRESSED_BZ2:
			if (!PHAR_G(has_bz2)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
					"Cannot compress files within archive with bz2, enable ext/bz2 in php.ini");
				return;
			}
			flags = PHAR_ENT_COMPRESSED_BZ2;
			break;
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
				"Unknown compression specified, please pass one of Phar::GZ or Phar::BZ2");
			return;
	}

	if (phar_obj->archive->is_tar) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
			"Cannot compress with Gzip compression, tar archives cannot compress individual files, use compress() to compress the whole archive");
		return;
	}

	if (!pharobj_cancompress(&phar_obj->archive->manifest)) {
		if (flags == PHAR_FILE_COMPRESSED_GZ) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
				"Cannot compress all files as Gzip, some are compressed as bzip2 and cannot be decompressed");
		} else {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0,
				"Cannot compress all files as Bzip2, some are compressed as gzip and cannot be decompressed");
		}
		return;
	}

	if (phar_obj->archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->archive))) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" is persistent, unable to copy on write", phar_obj->archive->fname);
		return;
	}
	pharobj_set_compression(&phar_obj->archive->manifest, flags);
	phar_obj->archive->is_modified = 1;
	phar_flush(phar_obj->archive, 0, 0, 0, &error);

	if (error) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "%s", error);
		efree(error);
	}
}