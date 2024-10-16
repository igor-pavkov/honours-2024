PHP_METHOD(Phar, webPhar)
{
	zval *mimeoverride = NULL, *rewrite = NULL;
	char *alias = NULL, *error, *index_php = NULL, *f404 = NULL, *ru = NULL;
	size_t alias_len = 0, f404_len = 0, free_pathinfo = 0;
	int  ru_len = 0;
	char *fname, *path_info, *mime_type = NULL, *entry, *pt;
	const char *basename;
	size_t fname_len, index_php_len = 0;
	int entry_len, code, not_cgi;
	phar_archive_data *phar = NULL;
	phar_entry_info *info = NULL;
	size_t sapi_mod_name_len = strlen(sapi_module.name);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s!s!saz", &alias, &alias_len, &index_php, &index_php_len, &f404, &f404_len, &mimeoverride, &rewrite) == FAILURE) {
		return;
	}

	phar_request_initialize();
	fname = (char*)zend_get_executed_filename();
	fname_len = strlen(fname);

	if (ZEND_SIZE_T_INT_OVFL(alias_len)
			|| ZEND_SIZE_T_INT_OVFL(f404_len) || ZEND_SIZE_T_INT_OVFL(index_php_len)) {
		RETURN_FALSE;
	}

	if (phar_open_executed_filename(alias, (int)alias_len, &error) != SUCCESS) {
		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
			efree(error);
		}
		return;
	}

	/* retrieve requested file within phar */
	if (!(SG(request_info).request_method
          && SG(request_info).request_uri
          && (!strcmp(SG(request_info).request_method, "GET")
           || !strcmp(SG(request_info).request_method, "POST")
           || !strcmp(SG(request_info).request_method, "DELETE")
           || !strcmp(SG(request_info).request_method, "HEAD")
           || !strcmp(SG(request_info).request_method, "OPTIONS")
           || !strcmp(SG(request_info).request_method, "PATCH")
           || !strcmp(SG(request_info).request_method, "PUT")
          )
         )
      ) {
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

	if ((sapi_mod_name_len == sizeof("cgi-fcgi") - 1 && !strncmp(sapi_module.name, "cgi-fcgi", sizeof("cgi-fcgi") - 1))
		|| (sapi_mod_name_len == sizeof("fpm-fcgi") - 1 && !strncmp(sapi_module.name, "fpm-fcgi", sizeof("fpm-fcgi") - 1))
		|| (sapi_mod_name_len == sizeof("cgi") - 1 && !strncmp(sapi_module.name, "cgi", sizeof("cgi") - 1))) {

		if (Z_TYPE(PG(http_globals)[TRACK_VARS_SERVER]) != IS_UNDEF) {
			HashTable *_server = Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]);
			zval *z_script_name, *z_path_info;

			if (NULL == (z_script_name = zend_hash_str_find(_server, "SCRIPT_NAME", sizeof("SCRIPT_NAME")-1)) ||
				IS_STRING != Z_TYPE_P(z_script_name) ||
				!strstr(Z_STRVAL_P(z_script_name), basename)) {
				return;
			}

			if (NULL != (z_path_info = zend_hash_str_find(_server, "PATH_INFO", sizeof("PATH_INFO")-1)) &&
				IS_STRING == Z_TYPE_P(z_path_info)) {
				entry_len = (int)Z_STRLEN_P(z_path_info);
				entry = estrndup(Z_STRVAL_P(z_path_info), entry_len);
				path_info = emalloc(Z_STRLEN_P(z_script_name) + entry_len + 1);
				memcpy(path_info, Z_STRVAL_P(z_script_name), Z_STRLEN_P(z_script_name));
				memcpy(path_info + Z_STRLEN_P(z_script_name), entry, entry_len + 1);
				free_pathinfo = 1;
			} else {
				entry_len = 0;
				entry = estrndup("", 0);
				path_info = Z_STRVAL_P(z_script_name);
			}

			pt = estrndup(Z_STRVAL_P(z_script_name), Z_STRLEN_P(z_script_name));

		} else {
			char *testit;

			testit = sapi_getenv("SCRIPT_NAME", sizeof("SCRIPT_NAME")-1);
			if (!(pt = strstr(testit, basename))) {
				efree(testit);
				return;
			}

			path_info = sapi_getenv("PATH_INFO", sizeof("PATH_INFO")-1);

			if (path_info) {
				entry = path_info;
				entry_len = (int)strlen(entry);
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

		entry_len = (int)strlen(path_info);
		entry_len -= (pt - path_info) + (fname_len - (basename - fname));
		entry = estrndup(pt + (fname_len - (basename - fname)), entry_len);

		pt = estrndup(path_info, (pt - path_info) + (fname_len - (basename - fname)));
		not_cgi = 1;
	}

	if (rewrite) {
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;
		zval params, retval;

		ZVAL_STRINGL(&params, entry, entry_len);

		if (FAILURE == zend_fcall_info_init(rewrite, 0, &fci, &fcc, NULL, NULL)) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "phar error: invalid rewrite callback");

			if (free_pathinfo) {
				efree(path_info);
			}
			efree(pt);

			return;
		}

		fci.param_count = 1;
		fci.params = &params;
		Z_ADDREF(params);
		fci.retval = &retval;

		if (FAILURE == zend_call_function(&fci, &fcc)) {
			if (!EG(exception)) {
				zend_throw_exception_ex(phar_ce_PharException, 0, "phar error: failed to call rewrite callback");
			}

			if (free_pathinfo) {
				efree(path_info);
			}
			efree(pt);

			return;
		}

		if (Z_TYPE_P(fci.retval) == IS_UNDEF || Z_TYPE(retval) == IS_UNDEF) {
			if (free_pathinfo) {
				efree(path_info);
			}
			zend_throw_exception_ex(phar_ce_PharException, 0, "phar error: rewrite callback must return a string or false");
			efree(pt);
			return;
		}

		switch (Z_TYPE(retval)) {
			case IS_STRING:
				efree(entry);
				if (ZEND_SIZE_T_INT_OVFL(Z_STRLEN_P(fci.retval))) {
					zend_throw_exception_ex(phar_ce_PharException, 0, "phar error: rewrite callback returned oversized value");
					return;
				}
				entry = estrndup(Z_STRVAL_P(fci.retval), Z_STRLEN_P(fci.retval));
				entry_len = (int)Z_STRLEN_P(fci.retval);
				break;
			case IS_TRUE:
			case IS_FALSE:
				phar_do_403(entry, entry_len);

				if (free_pathinfo) {
					efree(path_info);
				}
				efree(pt);

				zend_bailout();
				return;
			default:
				if (free_pathinfo) {
					efree(path_info);
				}
				efree(pt);

				zend_throw_exception_ex(phar_ce_PharException, 0, "phar error: rewrite callback must return a string or false");
				return;
		}
	}

	if (entry_len) {
		phar_postprocess_ru_web(fname, (int)fname_len, &entry, &entry_len, &ru, &ru_len);
	}

	if (!entry_len || (entry_len == 1 && entry[0] == '/')) {
		efree(entry);
		/* direct request */
		if (index_php_len) {
			entry = index_php;
			entry_len = (int)index_php_len;
			if (entry[0] != '/') {
				spprintf(&entry, 0, "/%s", index_php);
				++entry_len;
			}
		} else {
			/* assume "index.php" is starting point */
			entry = estrndup("/index.php", sizeof("/index.php"));
			entry_len = sizeof("/index.php")-1;
		}

		if (FAILURE == phar_get_archive(&phar, fname, (int)fname_len, NULL, 0, NULL) ||
			(info = phar_get_entry_info(phar, entry, entry_len, NULL, 0)) == NULL) {
			phar_do_404(phar, fname, (int)fname_len, f404, (int)f404_len, entry, entry_len);

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
			sapi_header_op(SAPI_HEADER_REPLACE, &ctr);

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

			sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
			sapi_send_headers();
			efree(ctr.line);
			zend_bailout();
		}
	}

	if (FAILURE == phar_get_archive(&phar, fname, (int)fname_len, NULL, 0, NULL) ||
		(info = phar_get_entry_info(phar, entry, entry_len, NULL, 0)) == NULL) {
		phar_do_404(phar, fname, (int)fname_len, f404, (int)f404_len, entry, entry_len);
#ifdef PHP_WIN32
		efree(fname);
#endif
		zend_bailout();
	}

	if (mimeoverride && zend_hash_num_elements(Z_ARRVAL_P(mimeoverride))) {
		const char *ext = zend_memrchr(entry, '.', entry_len);
		zval *val;

		if (ext) {
			++ext;

			if (NULL != (val = zend_hash_str_find(Z_ARRVAL_P(mimeoverride), ext, strlen(ext)))) {
				switch (Z_TYPE_P(val)) {
					case IS_LONG:
						if (Z_LVAL_P(val) == PHAR_MIME_PHP || Z_LVAL_P(val) == PHAR_MIME_PHPS) {
							mime_type = "";
							code = (int)Z_LVAL_P(val);
						} else {
							zend_throw_exception_ex(phar_ce_PharException, 0, "Unknown mime type specifier used, only Phar::PHP, Phar::PHPS and a mime type string are allowed");
							if (free_pathinfo) {
								efree(path_info);
							}
							efree(pt);
							efree(entry);
#ifdef PHP_WIN32
							efree(fname);
#endif
							RETURN_FALSE;
						}
						break;
					case IS_STRING:
						mime_type = Z_STRVAL_P(val);
						code = PHAR_MIME_OTHER;
						break;
					default:
						zend_throw_exception_ex(phar_ce_PharException, 0, "Unknown mime type specifier used (not a string or int), only Phar::PHP, Phar::PHPS and a mime type string are allowed");
						if (free_pathinfo) {
							efree(path_info);
						}
						efree(pt);
						efree(entry);
#ifdef PHP_WIN32
						efree(fname);
#endif
						RETURN_FALSE;
				}
			}
		}
	}

	if (!mime_type) {
		code = phar_file_type(&PHAR_G(mime_types), entry, &mime_type);
	}
	phar_file_action(phar, info, mime_type, code, entry, entry_len, fname, pt, ru, ru_len);
}