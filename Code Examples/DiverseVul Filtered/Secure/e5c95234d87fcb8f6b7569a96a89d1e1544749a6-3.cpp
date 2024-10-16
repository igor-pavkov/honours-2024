static int phar_file_action(phar_archive_data *phar, phar_entry_info *info, char *mime_type, int code, char *entry, int entry_len, char *arch, char *basename, char *ru, int ru_len) /* {{{ */
{
	char *name = NULL, buf[8192];
	const char *cwd;
	zend_syntax_highlighter_ini syntax_highlighter_ini;
	sapi_header_line ctr = {0};
	size_t got;
	zval dummy;
	size_t name_len;
	zend_file_handle file_handle;
	zend_op_array *new_op_array;
	zval result;
	php_stream *fp;
	zend_off_t position;

	switch (code) {
		case PHAR_MIME_PHPS:
			efree(basename);
			/* highlight source */
			if (entry[0] == '/') {
				spprintf(&name, 4096, "phar://%s%s", arch, entry);
			} else {
				spprintf(&name, 4096, "phar://%s/%s", arch, entry);
			}
			php_get_highlight_struct(&syntax_highlighter_ini);

			highlight_file(name, &syntax_highlighter_ini);

			efree(name);
#ifdef PHP_WIN32
			efree(arch);
#endif
			zend_bailout();
		case PHAR_MIME_OTHER:
			/* send headers, output file contents */
			efree(basename);
			ctr.line_len = spprintf(&(ctr.line), 0, "Content-type: %s", mime_type);
			sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
			efree(ctr.line);
			ctr.line_len = spprintf(&(ctr.line), 0, "Content-length: %u", info->uncompressed_filesize);
			sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
			efree(ctr.line);

			if (FAILURE == sapi_send_headers()) {
				zend_bailout();
			}

			/* prepare to output  */
			fp = phar_get_efp(info, 1);

			if (!fp) {
				char *error;
				if (!phar_open_jit(phar, info, &error)) {
					if (error) {
						zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
						efree(error);
					}
					return -1;
				}
				fp = phar_get_efp(info, 1);
			}
			position = 0;
			phar_seek_efp(info, 0, SEEK_SET, 0, 1);

			do {
				got = php_stream_read(fp, buf, MIN(8192, info->uncompressed_filesize - position));
				if (got > 0) {
					PHPWRITE(buf, got);
					position += got;
					if (position == (zend_off_t) info->uncompressed_filesize) {
						break;
					}
				}
			} while (1);

			zend_bailout();
		case PHAR_MIME_PHP:
			if (basename) {
				phar_mung_server_vars(arch, entry, entry_len, basename, ru_len);
				efree(basename);
			}

			if (entry[0] == '/') {
				name_len = spprintf(&name, 4096, "phar://%s%s", arch, entry);
			} else {
				name_len = spprintf(&name, 4096, "phar://%s/%s", arch, entry);
			}

			file_handle.type = ZEND_HANDLE_FILENAME;
			file_handle.handle.fd = 0;
			file_handle.filename = name;
			file_handle.opened_path = NULL;
			file_handle.free_filename = 0;

			PHAR_G(cwd) = NULL;
			PHAR_G(cwd_len) = 0;

			ZVAL_NULL(&dummy);
			if (zend_hash_str_add(&EG(included_files), name, name_len, &dummy) != NULL) {
				if ((cwd = zend_memrchr(entry, '/', entry_len))) {
					PHAR_G(cwd_init) = 1;
					if (entry == cwd) {
						/* root directory */
						PHAR_G(cwd_len) = 0;
						PHAR_G(cwd) = NULL;
					} else if (entry[0] == '/') {
						PHAR_G(cwd_len) = (int)(cwd - (entry + 1));
						PHAR_G(cwd) = estrndup(entry + 1, PHAR_G(cwd_len));
					} else {
						PHAR_G(cwd_len) = (int)(cwd - entry);
						PHAR_G(cwd) = estrndup(entry, PHAR_G(cwd_len));
					}
				}

				new_op_array = zend_compile_file(&file_handle, ZEND_REQUIRE);

				if (!new_op_array) {
					zend_hash_str_del(&EG(included_files), name, name_len);
				}

				zend_destroy_file_handle(&file_handle);

			} else {
				efree(name);
				new_op_array = NULL;
			}
#ifdef PHP_WIN32
			efree(arch);
#endif
			if (new_op_array) {
				ZVAL_UNDEF(&result);

				zend_try {
					zend_execute(new_op_array, &result);
					if (PHAR_G(cwd)) {
						efree(PHAR_G(cwd));
						PHAR_G(cwd) = NULL;
						PHAR_G(cwd_len) = 0;
					}

					PHAR_G(cwd_init) = 0;
					efree(name);
					destroy_op_array(new_op_array);
					efree(new_op_array);
					zval_ptr_dtor(&result);
				} zend_catch {
					if (PHAR_G(cwd)) {
						efree(PHAR_G(cwd));
						PHAR_G(cwd) = NULL;
						PHAR_G(cwd_len) = 0;
					}

					PHAR_G(cwd_init) = 0;
					efree(name);
				} zend_end_try();

				zend_bailout();
			}

			return PHAR_MIME_PHP;
	}
	return -1;
}