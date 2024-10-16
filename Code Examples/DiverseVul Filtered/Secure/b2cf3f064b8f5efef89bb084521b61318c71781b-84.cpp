static void phar_mung_server_vars(char *fname, char *entry, int entry_len, char *basename, int request_uri_len TSRMLS_DC) /* {{{ */
{
	HashTable *_SERVER;
	zval **stuff;
	char *path_info;
	int basename_len = strlen(basename);
	int code;
	zval *temp;

	/* "tweak" $_SERVER variables requested in earlier call to Phar::mungServer() */
	if (!PG(http_globals)[TRACK_VARS_SERVER]) {
		return;
	}

	_SERVER = Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_SERVER]);

	/* PATH_INFO and PATH_TRANSLATED should always be munged */
	if (SUCCESS == zend_hash_find(_SERVER, "PATH_INFO", sizeof("PATH_INFO"), (void **) &stuff)) {
		path_info = Z_STRVAL_PP(stuff);
		code = Z_STRLEN_PP(stuff);

		if (Z_STRLEN_PP(stuff) > entry_len && !memcmp(Z_STRVAL_PP(stuff), entry, entry_len)) {
			ZVAL_STRINGL(*stuff, Z_STRVAL_PP(stuff) + entry_len, request_uri_len, 1);

			MAKE_STD_ZVAL(temp);
			ZVAL_STRINGL(temp, path_info, code, 0);

			zend_hash_update(_SERVER, "PHAR_PATH_INFO", sizeof("PHAR_PATH_INFO"), &temp, sizeof(zval **), NULL);
		}
	}

	if (SUCCESS == zend_hash_find(_SERVER, "PATH_TRANSLATED", sizeof("PATH_TRANSLATED"), (void **) &stuff)) {
		path_info = Z_STRVAL_PP(stuff);
		code = Z_STRLEN_PP(stuff);
		Z_STRLEN_PP(stuff) = spprintf(&(Z_STRVAL_PP(stuff)), 4096, "phar://%s%s", fname, entry);

		MAKE_STD_ZVAL(temp);
		ZVAL_STRINGL(temp, path_info, code, 0);

		zend_hash_update(_SERVER, "PHAR_PATH_TRANSLATED", sizeof("PHAR_PATH_TRANSLATED"), (void *) &temp, sizeof(zval **), NULL);
	}

	if (!PHAR_GLOBALS->phar_SERVER_mung_list) {
		return;
	}

	if (PHAR_GLOBALS->phar_SERVER_mung_list & PHAR_MUNG_REQUEST_URI) {
		if (SUCCESS == zend_hash_find(_SERVER, "REQUEST_URI", sizeof("REQUEST_URI"), (void **) &stuff)) {
			path_info = Z_STRVAL_PP(stuff);
			code = Z_STRLEN_PP(stuff);

			if (Z_STRLEN_PP(stuff) > basename_len && !memcmp(Z_STRVAL_PP(stuff), basename, basename_len)) {
				ZVAL_STRINGL(*stuff, Z_STRVAL_PP(stuff) + basename_len, Z_STRLEN_PP(stuff) - basename_len, 1);

				MAKE_STD_ZVAL(temp);
				ZVAL_STRINGL(temp, path_info, code, 0);

				zend_hash_update(_SERVER, "PHAR_REQUEST_URI", sizeof("PHAR_REQUEST_URI"), (void *) &temp, sizeof(zval **), NULL);
			}
		}
	}

	if (PHAR_GLOBALS->phar_SERVER_mung_list & PHAR_MUNG_PHP_SELF) {
		if (SUCCESS == zend_hash_find(_SERVER, "PHP_SELF", sizeof("PHP_SELF"), (void **) &stuff)) {
			path_info = Z_STRVAL_PP(stuff);
			code = Z_STRLEN_PP(stuff);

			if (Z_STRLEN_PP(stuff) > basename_len && !memcmp(Z_STRVAL_PP(stuff), basename, basename_len)) {
				ZVAL_STRINGL(*stuff, Z_STRVAL_PP(stuff) + basename_len, Z_STRLEN_PP(stuff) - basename_len, 1);

				MAKE_STD_ZVAL(temp);
				ZVAL_STRINGL(temp, path_info, code, 0);

				zend_hash_update(_SERVER, "PHAR_PHP_SELF", sizeof("PHAR_PHP_SELF"), (void *) &temp, sizeof(zval **), NULL);
			}
		}
	}

	if (PHAR_GLOBALS->phar_SERVER_mung_list & PHAR_MUNG_SCRIPT_NAME) {
		if (SUCCESS == zend_hash_find(_SERVER, "SCRIPT_NAME", sizeof("SCRIPT_NAME"), (void **) &stuff)) {
			path_info = Z_STRVAL_PP(stuff);
			code = Z_STRLEN_PP(stuff);
			ZVAL_STRINGL(*stuff, entry, entry_len, 1);

			MAKE_STD_ZVAL(temp);
			ZVAL_STRINGL(temp, path_info, code, 0);

			zend_hash_update(_SERVER, "PHAR_SCRIPT_NAME", sizeof("PHAR_SCRIPT_NAME"), (void *) &temp, sizeof(zval **), NULL);
		}
	}

	if (PHAR_GLOBALS->phar_SERVER_mung_list & PHAR_MUNG_SCRIPT_FILENAME) {
		if (SUCCESS == zend_hash_find(_SERVER, "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME"), (void **) &stuff)) {
			path_info = Z_STRVAL_PP(stuff);
			code = Z_STRLEN_PP(stuff);
			Z_STRLEN_PP(stuff) = spprintf(&(Z_STRVAL_PP(stuff)), 4096, "phar://%s%s", fname, entry);

			MAKE_STD_ZVAL(temp);
			ZVAL_STRINGL(temp, path_info, code, 0);

			zend_hash_update(_SERVER, "PHAR_SCRIPT_FILENAME", sizeof("PHAR_SCRIPT_FILENAME"), (void *) &temp, sizeof(zval **), NULL);
		}
	}
}