PHP_METHOD(Phar, extractTo)
{
	php_stream *fp;
	php_stream_statbuf ssb;
	char *pathto;
	zend_string *filename;
	size_t pathto_len;
	int ret, i;
	int nelems;
	zval *zval_file;
	zval *zval_files = NULL;
	zend_bool overwrite = 0;
	char *error = NULL;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p|z!b", &pathto, &pathto_len, &zval_files, &overwrite) == FAILURE) {
		return;
	}

	fp = php_stream_open_wrapper(phar_obj->archive->fname, "rb", IGNORE_URL|STREAM_MUST_SEEK, NULL);

	if (!fp) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,
			"Invalid argument, %s cannot be found", phar_obj->archive->fname);
		return;
	}

	php_stream_close(fp);

	if (pathto_len < 1) {
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,
			"Invalid argument, extraction path must be non-zero length");
		return;
	}

	if (pathto_len >= MAXPATHLEN) {
		char *tmp = estrndup(pathto, 50);
		/* truncate for error message */
		zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Cannot extract to \"%s...\", destination directory is too long for filesystem", tmp);
		efree(tmp);
		return;
	}

	if (php_stream_stat_path(pathto, &ssb) < 0) {
		ret = php_stream_mkdir(pathto, 0777,  PHP_STREAM_MKDIR_RECURSIVE, NULL);
		if (!ret) {
			zend_throw_exception_ex(spl_ce_RuntimeException, 0,
				"Unable to create path \"%s\" for extraction", pathto);
			return;
		}
	} else if (!(ssb.sb.st_mode & S_IFDIR)) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0,
			"Unable to use path \"%s\" for extraction, it is a file, must be a directory", pathto);
		return;
	}

	if (zval_files) {
		switch (Z_TYPE_P(zval_files)) {
			case IS_NULL:
				filename = NULL;
				break;
			case IS_STRING:
				filename = Z_STR_P(zval_files);
				break;
			case IS_ARRAY:
				nelems = zend_hash_num_elements(Z_ARRVAL_P(zval_files));
				if (nelems == 0 ) {
					RETURN_FALSE;
				}
				for (i = 0; i < nelems; i++) {
					if ((zval_file = zend_hash_index_find(Z_ARRVAL_P(zval_files), i)) != NULL) {
						if (IS_STRING != Z_TYPE_P(zval_file)) {
							zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,
								"Invalid argument, array of filenames to extract contains non-string value");
							return;
						}
						switch (extract_helper(phar_obj->archive, Z_STR_P(zval_file), pathto, pathto_len, overwrite, &error)) {
							case -1:
								zend_throw_exception_ex(phar_ce_PharException, 0, "Extraction from phar \"%s\" failed: %s",
									phar_obj->archive->fname, error);
								efree(error);
								return;
							case 0:
								zend_throw_exception_ex(phar_ce_PharException, 0,
									"Phar Error: attempted to extract non-existent file or directory \"%s\" from phar \"%s\"",
									ZSTR_VAL(Z_STR_P(zval_file)), phar_obj->archive->fname);
								return;
						}
					}
				}
				RETURN_TRUE;
			default:
				zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0,
					"Invalid argument, expected a filename (string) or array of filenames");
				return;
		}
	} else {
		filename = NULL;
	}

	ret = extract_helper(phar_obj->archive, filename, pathto, pathto_len, overwrite, &error);
	if (-1 == ret) {
		zend_throw_exception_ex(phar_ce_PharException, 0, "Extraction from phar \"%s\" failed: %s",
			phar_obj->archive->fname, error);
		efree(error);
	} else if (0 == ret && NULL != filename) {
		zend_throw_exception_ex(phar_ce_PharException, 0,
			"Phar Error: attempted to extract non-existent file or directory \"%s\" from phar \"%s\"",
			ZSTR_VAL(filename), phar_obj->archive->fname);
	} else {
		RETURN_TRUE;
	}
}