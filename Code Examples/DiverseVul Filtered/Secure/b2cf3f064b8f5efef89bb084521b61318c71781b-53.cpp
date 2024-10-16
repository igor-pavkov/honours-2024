PHP_METHOD(Phar, webPhar)
{
	zval *mimeoverride = NULL, *rewrite = NULL;
	char *alias = NULL, *error, *index_php = NULL, *f404 = NULL, *ru = NULL;
	int alias_len = 0, ret, f404_len = 0, free_pathinfo = 0, ru_len = 0;
	char *fname, *path_info, *mime_type = NULL, *entry, *pt;
	const char *basename;
	int fname_len, entry_len, code, index_php_len = 0, not_cgi;
	phar_archive_data *phar = NULL;
	phar_entry_info *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!s!saz", &alias, &alias_len, &index_php, &index_php_len, &f404, &f404_len, &mimeoverride, &rewrite) == FAILURE) {
		return;
	}

	phar_request_initialize(TSRMLS_C);
	fname = (char*)zend_get_executed_filename(TSRMLS_C);
	fname_len = strlen(fname);

	if (phar_open_executed_filename(alias, alias_len, &error TSRMLS_CC) != SUCCESS) {
		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
		}
		return;
	}

	/* retrieve requested file within phar */
	if (!(SG(request_info).request_method && SG(request_info).request_uri && (!strcmp(SG(request_info).request_method, "GET") || !strcmp(SG(request_info).request_method, "POST")))) {
		return;
	}

#ifdef PHP_WIN32
	fname = estrndup(fname, fname_len);
	phar_unixify_path_separators(fname, fname_len);
#endif
	basename = zend_memrchr(fname, '/', fname_len);

	if (!basename) {
		basename = fname;
	} else {
		++basename;
	}

	if ((strlen(sapi_module.name) == sizeof("cgi-fcgi")-1 && !strncmp(sapi_module.name, "cgi-fcgi", sizeof("cgi-fcgi")-1))
		|| (strlen(sapi_module.name) == sizeof("fpm-fcgi")-1 && !strncmp(sapi_module.name, "fpm-fcgi", sizeof("fpm-fcgi")-1))
		|| (strlen(sapi_module.name) == sizeof("cgi")-1 && !strncmp(sapi_module.name, "cgi", sizeof("cgi")-1))) {

		if (PG(http_globals)[TRACK_VARS_SERVER]) {
			HashTable *_server = Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_SERVER]);
			zval **z_script_name, **z_path_info;

			if (SUCCESS != zend_hash_find(_server, "SCRIPT_NAME", sizeof("SCRIPT_NAME"), (void**)&z_script_name) ||
				IS_STRING != Z_TYPE_PP(z_script_name) ||
				!strstr(Z_STRVAL_PP(z_script_name), basename)) {
				return;
			}

			if (SUCCESS == zend_hash_find(_server, "PATH_INFO", sizeof("PATH_INFO"), (void**)&z_path_info) &&
				IS_STRING == Z_TYPE_PP(z_path_info)) {
				entry_len = Z_STRLEN_PP(z_path_info);
				entry = estrndup(Z_STRVAL_PP(z_path_info), entry_len);
				path_info = emalloc(Z_STRLEN_PP(z_script_name) + entry_len + 1);
				memcpy(path_info, Z_STRVAL_PP(z_script_name), Z_STRLEN_PP(z_script_name));
				memcpy(path_info + Z_STRLEN_PP(z_script_name), entry, entry_len + 1);
				free_pathinfo = 1;
			} else {
				entry_len = 0;
				entry = estrndup("", 0);
				path_info = Z_STRVAL_PP(z_script_name);
			}

			pt = estrndup(Z_STRVAL_PP(z_script_name), Z_STRLEN_PP(z_script_name));

		} else {
			char *testit;

			testit = sapi_getenv("SCRIPT_NAME", sizeof("SCRIPT_NAME")-1 TSRMLS_CC);
			if (!(pt = strstr(testit, basename))) {
				efree(testit);
				return;
			}

			path_info = sapi_getenv("PATH_INFO", sizeof("PATH_INFO")-1 TSRMLS_CC);

			if (path_info) {
				entry = path_info;
				entry_len = strlen(entry);
				spprintf(&path_info, 0, "%s%s", testit, path_info);
				free_pathinfo = 1;
			} else {
				path_info = testit;
				free_pathinfo = 1;
				entry = estrndup("", 0);
				entry_len = 0;
			}

			pt = estrndup(testit, (pt - testit) + (fname_len - (basename - fname)));
		}
		not_cgi = 0;
	} else {
		path_info = SG(request_info).request_uri;

		if (!(pt = strstr(path_info, basename))) {
			/* this can happen with rewrite rules - and we have no idea what to do then, so return */
			return;
		}

		entry_len = strlen(path_info);
		entry_len -= (pt - path_info) + (fname_len - (basename - fname));
		entry = estrndup(pt + (fname_len - (basename - fname)), entry_len);

		pt = estrndup(path_info, (pt - path_info) + (fname_len - (basename - fname)));
		not_cgi = 1;
	}

	if (rewrite) {
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;
		zval *params, *retval_ptr, **zp[1];

		MAKE_STD_ZVAL(params);
		ZVAL_STRINGL(params, entry, entry_len, 1);
		zp[0] = &params;

		if (FAILURE == zend_fcall_info_init(rewrite, 0, &fci, &fcc, NULL, NULL TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar error: invalid rewrite callback");

			if (free_pathinfo) {
				efree(path_info);
			}

			return;
		}

		fci.param_count = 1;
		fci.params = zp;
		Z_ADDREF_P(params);
		fci.retval_ptr_ptr = &retval_ptr;

		if (FAILURE == zend_call_function(&fci, &fcc TSRMLS_CC)) {
			if (!EG(exception)) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar error: failed to call rewrite callback");
			}

			if (free_pathinfo) {
				efree(path_info);
			}

			return;
		}

		if (!fci.retval_ptr_ptr || !retval_ptr) {
			if (free_pathinfo) {
				efree(path_info);
			}
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar error: rewrite callback must return a string or false");
			return;
		}

		switch (Z_TYPE_P(retval_ptr)) {
			case IS_STRING:
				efree(entry);

				if (fci.retval_ptr_ptr != &retval_ptr) {
					entry = estrndup(Z_STRVAL_PP(fci.retval_ptr_ptr), Z_STRLEN_PP(fci.retval_ptr_ptr));
					entry_len = Z_STRLEN_PP(fci.retval_ptr_ptr);
				} else {
					entry = Z_STRVAL_P(retval_ptr);
					entry_len = Z_STRLEN_P(retval_ptr);
				}

				break;
			case IS_BOOL:
				phar_do_403(entry, entry_len TSRMLS_CC);

				if (free_pathinfo) {
					efree(path_info);
				}

				zend_bailout();
				return;
			default:
				efree(retval_ptr);

				if (free_pathinfo) {
					efree(path_info);
				}

				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar error: rewrite callback must return a string or false");
				return;
		}
	}

	if (entry_len) {
		phar_postprocess_ru_web(fname, fname_len, &entry, &entry_len, &ru, &ru_len TSRMLS_CC);
	}

	if (!entry_len || (entry_len == 1 && entry[0] == '/')) {
		efree(entry);
		/* direct request */
		if (index_php_len) {
			entry = index_php;
			entry_len = index_php_len;
			if (entry[0] != '/') {
				spprintf(&entry, 0, "/%s", index_php);
				++entry_len;
			}
		} else {
			/* assume "index.php" is starting point */
			entry = estrndup("/index.php", sizeof("/index.php"));
			entry_len = sizeof("/index.php")-1;
		}

		if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, NULL TSRMLS_CC) ||
			(info = phar_get_entry_info(phar, entry, entry_len, NULL, 0 TSRMLS_CC)) == NULL) {
			phar_do_404(phar, fname, fname_len, f404, f404_len, entry, entry_len TSRMLS_CC);

			if (free_pathinfo) {
				efree(path_info);
			}

			zend_bailout();
		} else {
			char *tmp = NULL, sa = '\0';
			sapi_header_line ctr = {0};
			ctr.response_code = 301;
			ctr.line_len = sizeof("HTTP/1.1 301 Moved Permanently")-1;
			ctr.line = "HTTP/1.1 301 Moved Permanently";
			sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);

			if (not_cgi) {
				tmp = strstr(path_info, basename) + fname_len;
				sa = *tmp;
				*tmp = '\0';
			}

			ctr.response_code = 0;

			if (path_info[strlen(path_info)-1] == '/') {
				ctr.line_len = spprintf(&(ctr.line), 4096, "Location: %s%s", path_info, entry + 1);
			} else {
				ctr.line_len = spprintf(&(ctr.line), 4096, "Location: %s%s", path_info, entry);
			}

			if (not_cgi) {
				*tmp = sa;
			}

			if (free_pathinfo) {
				efree(path_info);
			}

			sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC);
			sapi_send_headers(TSRMLS_C);
			efree(ctr.line);
			zend_bailout();
		}
	}

	if (FAILURE == phar_get_archive(&phar, fname, fname_len, NULL, 0, NULL TSRMLS_CC) ||
		(info = phar_get_entry_info(phar, entry, entry_len, NULL, 0 TSRMLS_CC)) == NULL) {
		phar_do_404(phar, fname, fname_len, f404, f404_len, entry, entry_len TSRMLS_CC);
#ifdef PHP_WIN32
		efree(fname);
#endif
		zend_bailout();
	}

	if (mimeoverride && zend_hash_num_elements(Z_ARRVAL_P(mimeoverride))) {
		const char *ext = zend_memrchr(entry, '.', entry_len);
		zval **val;

		if (ext) {
			++ext;

			if (SUCCESS == zend_hash_find(Z_ARRVAL_P(mimeoverride), ext, strlen(ext)+1, (void **) &val)) {
				switch (Z_TYPE_PP(val)) {
					case IS_LONG:
						if (Z_LVAL_PP(val) == PHAR_MIME_PHP || Z_LVAL_PP(val) == PHAR_MIME_PHPS) {
							mime_type = "";
							code = Z_LVAL_PP(val);
						} else {
							zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown mime type specifier used, only Phar::PHP, Phar::PHPS and a mime type string are allowed");
#ifdef PHP_WIN32
							efree(fname);
#endif
							RETURN_FALSE;
						}
						break;
					case IS_STRING:
						mime_type = Z_STRVAL_PP(val);
						code = PHAR_MIME_OTHER;
						break;
					default:
						zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Unknown mime type specifier used (not a string or int), only Phar::PHP, Phar::PHPS and a mime type string are allowed");
#ifdef PHP_WIN32
						efree(fname);
#endif
						RETURN_FALSE;
				}
			}
		}
	}

	if (!mime_type) {
		code = phar_file_type(&PHAR_G(mime_types), entry, &mime_type TSRMLS_CC);
	}
	ret = phar_file_action(phar, info, mime_type, code, entry, entry_len, fname, pt, ru, ru_len TSRMLS_CC);
}