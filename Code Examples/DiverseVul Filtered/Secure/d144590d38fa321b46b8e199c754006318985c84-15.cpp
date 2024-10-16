PHP_METHOD(Phar, offsetUnset)
{
	char *fname, *error;
	int fname_len;
	phar_entry_info *entry;
	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &fname, &fname_len) == FAILURE) {
		return;
	}

	if (zend_hash_exists(&phar_obj->arc.archive->manifest, fname, (uint) fname_len)) {
		if (SUCCESS == zend_hash_find(&phar_obj->arc.archive->manifest, fname, (uint) fname_len, (void**)&entry)) {
			if (entry->is_deleted) {
				/* entry is deleted, but has not been flushed to disk yet */
				return;
			}

			if (phar_obj->arc.archive->is_persistent) {
				if (FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
					zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
					return;
				}
				/* re-populate entry after copy on write */
				zend_hash_find(&phar_obj->arc.archive->manifest, fname, (uint) fname_len, (void **)&entry);
			}
			entry->is_modified = 0;
			entry->is_deleted = 1;
			/* we need to "flush" the stream to save the newly deleted file on disk */
			phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

			if (error) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
				efree(error);
			}

			RETURN_TRUE;
		}
	} else {
		RETURN_FALSE;
	}
}