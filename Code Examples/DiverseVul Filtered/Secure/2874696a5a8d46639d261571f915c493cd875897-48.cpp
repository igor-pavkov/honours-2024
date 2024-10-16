static X509_STORE * setup_verify(zval * calist TSRMLS_DC)
{
	X509_STORE *store;
	X509_LOOKUP * dir_lookup, * file_lookup;
	HashPosition pos;
	int ndirs = 0, nfiles = 0;

	store = X509_STORE_new();

	if (store == NULL) {
		return NULL;
	}

	if (calist && (Z_TYPE_P(calist) == IS_ARRAY)) {
		zend_hash_internal_pointer_reset_ex(HASH_OF(calist), &pos);
		for (;; zend_hash_move_forward_ex(HASH_OF(calist), &pos)) {
			zval ** item;
			struct stat sb;

			if (zend_hash_get_current_data_ex(HASH_OF(calist), (void**)&item, &pos) == FAILURE) {
				break;
			}
			convert_to_string_ex(item);

			if (VCWD_STAT(Z_STRVAL_PP(item), &sb) == -1) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to stat %s", Z_STRVAL_PP(item));
				continue;
			}

			if ((sb.st_mode & S_IFREG) == S_IFREG) {
				file_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
				if (file_lookup == NULL || !X509_LOOKUP_load_file(file_lookup, Z_STRVAL_PP(item), X509_FILETYPE_PEM)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "error loading file %s", Z_STRVAL_PP(item));
				} else {
					nfiles++;
				}
				file_lookup = NULL;
			} else {
				dir_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
				if (dir_lookup == NULL || !X509_LOOKUP_add_dir(dir_lookup, Z_STRVAL_PP(item), X509_FILETYPE_PEM)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "error loading directory %s", Z_STRVAL_PP(item));
				} else {
					ndirs++;
				}
				dir_lookup = NULL;
			}
		}
	}
	if (nfiles == 0) {
		file_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
		if (file_lookup) {
			X509_LOOKUP_load_file(file_lookup, NULL, X509_FILETYPE_DEFAULT);
		}
	}
	if (ndirs == 0) {
		dir_lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
		if (dir_lookup) {
			X509_LOOKUP_add_dir(dir_lookup, NULL, X509_FILETYPE_DEFAULT);
		}
	}
	return store;
}