PHP_METHOD(Phar, delete)
{
	char *fname;
	int fname_len;
	char *error;
	phar_entry_info *entry;
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write out phar archive, phar is read-only");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &fname, &fname_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
		return;
	}
	if (zend_hash_exists(&phar_obj->arc.archive->manifest, fname, (uint) fname_len)) {
		if (SUCCESS == zend_hash_find(&phar_obj->arc.archive->manifest, fname, (uint) fname_len, (void**)&entry)) {
			if (entry->is_deleted) {
				/* entry is deleted, but has not been flushed to disk yet */
				RETURN_TRUE;
			} else {
				entry->is_deleted = 1;
				entry->is_modified = 1;
				phar_obj->arc.archive->is_modified = 1;
			}
		}
	} else {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Entry %s does not exist and cannot be deleted", fname);
		RETURN_FALSE;
	}

	phar_flush(phar_obj->arc.archive, NULL, 0, 0, &error TSRMLS_CC);
	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}

	RETURN_TRUE;
}