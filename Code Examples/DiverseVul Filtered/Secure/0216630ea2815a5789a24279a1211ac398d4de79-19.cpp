static X509_STORE *php_openssl_setup_verify(zval *calist)
{
	X509_STORE *store;
	X509_LOOKUP * dir_lookup, * file_lookup;
	int ndirs = 0, nfiles = 0;
	zval * item;
	zend_stat_t sb;

	store = X509_STORE_new();

	if (store == NULL) {
		php_openssl_store_errors();
		return NULL;
	}

	if (calist && (Z_TYPE_P(calist) == IS_ARRAY)) {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(calist), item) {
			convert_to_string_ex(item);

			if (VCWD_STAT(Z_STRVAL_P(item), &sb) == -1) {
				php_error_docref(NULL, E_WARNING, "unable to stat %s", Z_STRVAL_P(item));
				continue;
			}

			if ((sb.st_mode & S_IFREG) == S_IFREG) {
				file_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
				if (file_lookup == NULL || !X509_LOOKUP_load_file(file_lookup, Z_STRVAL_P(item), X509_FILETYPE_PEM)) {
					php_openssl_store_errors();
					php_error_docref(NULL, E_WARNING, "error loading file %s", Z_STRVAL_P(item));
				} else {
					nfiles++;
				}
				file_lookup = NULL;
			} else {
				dir_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
				if (dir_lookup == NULL || !X509_LOOKUP_add_dir(dir_lookup, Z_STRVAL_P(item), X509_FILETYPE_PEM)) {
					php_openssl_store_errors();
					php_error_docref(NULL, E_WARNING, "error loading directory %s", Z_STRVAL_P(item));
				} else {
					ndirs++;
				}
				dir_lookup = NULL;
			}
		} ZEND_HASH_FOREACH_END();
	}
	if (nfiles == 0) {
		file_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
		if (file_lookup == NULL || !X509_LOOKUP_load_file(file_lookup, NULL, X509_FILETYPE_DEFAULT)) {
			php_openssl_store_errors();
		}
	}
	if (ndirs == 0) {
		dir_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
		if (dir_lookup == NULL || !X509_LOOKUP_add_dir(dir_lookup, NULL, X509_FILETYPE_DEFAULT)) {
			php_openssl_store_errors();
		}
	}
	return store;
}