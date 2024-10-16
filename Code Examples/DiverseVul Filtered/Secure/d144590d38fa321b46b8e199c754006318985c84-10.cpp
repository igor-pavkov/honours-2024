PHP_METHOD(Phar, mount)
{
	char *fname, *arch = NULL, *entry = NULL, *path, *actual;
	int fname_len, arch_len, entry_len, path_len, actual_len;
	phar_archive_data **pphar;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "pp", &path, &path_len, &actual, &actual_len) == FAILURE) {
		return;
	}

	fname = (char*)zend_get_executed_filename(TSRMLS_C);
	fname_len = strlen(fname);

#ifdef PHP_WIN32
	phar_unixify_path_separators(fname, fname_len);
#endif

	if (fname_len > 7 && !memcmp(fname, "phar://", 7) && SUCCESS == phar_split_fname(fname, fname_len, &arch, &arch_len, &entry, &entry_len, 2, 0 TSRMLS_CC)) {
		efree(entry);
		entry = NULL;

		if (path_len > 7 && !memcmp(path, "phar://", 7)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Can only mount internal paths within a phar archive, use a relative path instead of \"%s\"", path);
			efree(arch);
			return;
		}
carry_on2:
		if (SUCCESS != zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), arch, arch_len, (void **)&pphar)) {
			if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_find(&cached_phars, arch, arch_len, (void **)&pphar)) {
				if (SUCCESS == phar_copy_on_write(pphar TSRMLS_CC)) {
					goto carry_on;
				}
			}

			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s is not a phar archive, cannot mount", arch);

			if (arch) {
				efree(arch);
			}
			return;
		}
carry_on:
		if (SUCCESS != phar_mount_entry(*pphar, actual, actual_len, path, path_len TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Mounting of %s to %s within phar %s failed", path, actual, arch);
			if (path && path == entry) {
				efree(entry);
			}

			if (arch) {
				efree(arch);
			}

			return;
		}

		if (entry && path && path == entry) {
			efree(entry);
		}

		if (arch) {
			efree(arch);
		}

		return;
	} else if (PHAR_GLOBALS->phar_fname_map.arBuckets && SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_fname_map), fname, fname_len, (void **)&pphar)) {
		goto carry_on;
	} else if (PHAR_G(manifest_cached) && SUCCESS == zend_hash_find(&cached_phars, fname, fname_len, (void **)&pphar)) {
		if (SUCCESS == phar_copy_on_write(pphar TSRMLS_CC)) {
			goto carry_on;
		}

		goto carry_on;
	} else if (SUCCESS == phar_split_fname(path, path_len, &arch, &arch_len, &entry, &entry_len, 2, 0 TSRMLS_CC)) {
		path = entry;
		path_len = entry_len;
		goto carry_on2;
	}

	zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Mounting of %s to %s failed", path, actual);
}