PHP_METHOD(Phar, extractTo)
{
	char *error = NULL;
	php_stream *fp;
	php_stream_statbuf ssb;
	phar_entry_info *entry;
	char *pathto, *filename, *actual;
	int pathto_len, filename_len;
	int ret, i;
	int nelems;
	zval *zval_files = NULL;
	zend_bool overwrite = 0;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|z!b", &pathto, &pathto_len, &zval_files, &overwrite) == FAILURE) {
		return;
	}

	fp = php_stream_open_wrapper(phar_obj->arc.archive->fname, "rb", IGNORE_URL|STREAM_MUST_SEEK, &actual);

	if (!fp) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC,
			"Invalid argument, %s cannot be found", phar_obj->arc.archive->fname);
		return;
	}

	efree(actual);
	php_stream_close(fp);

	if (pathto_len < 1) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC,
			"Invalid argument, extraction path must be non-zero length");
		return;
	}

	if (pathto_len >= MAXPATHLEN) {
		char *tmp = estrndup(pathto, 50);
		/* truncate for error message */
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC, "Cannot extract to \"%s...\", destination directory is too long for filesystem", tmp);
		efree(tmp);
		return;
	}

	if (php_stream_stat_path(pathto, &ssb) < 0) {
		ret = php_stream_mkdir(pathto, 0777,  PHP_STREAM_MKDIR_RECURSIVE, NULL);
		if (!ret) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
				"Unable to create path \"%s\" for extraction", pathto);
			return;
		}
	} else if (!(ssb.sb.st_mode & S_IFDIR)) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC,
			"Unable to use path \"%s\" for extraction, it is a file, must be a directory", pathto);
		return;
	}

	if (zval_files) {
		switch (Z_TYPE_P(zval_files)) {
			case IS_NULL:
				goto all_files;
			case IS_STRING:
				filename = Z_STRVAL_P(zval_files);
				filename_len = Z_STRLEN_P(zval_files);
				break;
			case IS_ARRAY:
				nelems = zend_hash_num_elements(Z_ARRVAL_P(zval_files));
				if (nelems == 0 ) {
					RETURN_FALSE;
				}
				for (i = 0; i < nelems; i++) {
					zval **zval_file;
					if (zend_hash_index_find(Z_ARRVAL_P(zval_files), i, (void **) &zval_file) == SUCCESS) {
						switch (Z_TYPE_PP(zval_file)) {
							case IS_STRING:
								break;
							default:
								zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC,
									"Invalid argument, array of filenames to extract contains non-string value");
								return;
						}
						if (FAILURE == zend_hash_find(&phar_obj->arc.archive->manifest, Z_STRVAL_PP(zval_file), Z_STRLEN_PP(zval_file), (void **)&entry)) {
							zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC,
								"Phar Error: attempted to extract non-existent file \"%s\" from phar \"%s\"", Z_STRVAL_PP(zval_file), phar_obj->arc.archive->fname);
						}
						if (FAILURE == phar_extract_file(overwrite, entry, pathto, pathto_len, &error TSRMLS_CC)) {
							zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC,
								"Extraction from phar \"%s\" failed: %s", phar_obj->arc.archive->fname, error);
							efree(error);
							return;
						}
					}
				}
				RETURN_TRUE;
			default:
				zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0 TSRMLS_CC,
					"Invalid argument, expected a filename (string) or array of filenames");
				return;
		}

		if (FAILURE == zend_hash_find(&phar_obj->arc.archive->manifest, filename, filename_len, (void **)&entry)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC,
				"Phar Error: attempted to extract non-existent file \"%s\" from phar \"%s\"", filename, phar_obj->arc.archive->fname);
			return;
		}

		if (FAILURE == phar_extract_file(overwrite, entry, pathto, pathto_len, &error TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC,
				"Extraction from phar \"%s\" failed: %s", phar_obj->arc.archive->fname, error);
			efree(error);
			return;
		}
	} else {
		phar_archive_data *phar;
all_files:
		phar = phar_obj->arc.archive;
		/* Extract all files */
		if (!zend_hash_num_elements(&(phar->manifest))) {
			RETURN_TRUE;
		}

		for (zend_hash_internal_pointer_reset(&phar->manifest);
		zend_hash_has_more_elements(&phar->manifest) == SUCCESS;
		zend_hash_move_forward(&phar->manifest)) {

			if (zend_hash_get_current_data(&phar->manifest, (void **)&entry) == FAILURE) {
				continue;
			}

			if (FAILURE == phar_extract_file(overwrite, entry, pathto, pathto_len, &error TSRMLS_CC)) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC,
					"Extraction from phar \"%s\" failed: %s", phar->fname, error);
				efree(error);
				return;
			}
		}
	}
	RETURN_TRUE;
}