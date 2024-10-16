PHP_METHOD(PharFileInfo, chmod)
{
	char *error;
	long perms;
	PHAR_ENTRY_OBJECT();

	if (entry_obj->ent.entry->is_temp_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry \"%s\" is a temporary directory (not an actual entry in the archive), cannot chmod", entry_obj->ent.entry->filename); \
		return;
	}

	if (PHAR_G(readonly) && !entry_obj->ent.entry->phar->is_data) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "Cannot modify permissions for file \"%s\" in phar \"%s\", write operations are prohibited", entry_obj->ent.entry->filename, entry_obj->ent.entry->phar->fname);
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &perms) == FAILURE) {
		return;
	}

	if (entry_obj->ent.entry->is_persistent) {
		phar_archive_data *phar = entry_obj->ent.entry->phar;

		if (FAILURE == phar_copy_on_write(&phar TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar->fname);
			return;
		}
		/* re-populate after copy-on-write */
		zend_hash_find(&phar->manifest, entry_obj->ent.entry->filename, entry_obj->ent.entry->filename_len, (void **)&entry_obj->ent.entry);
	}
	/* clear permissions */
	entry_obj->ent.entry->flags &= ~PHAR_ENT_PERM_MASK;
	perms &= 0777;
	entry_obj->ent.entry->flags |= perms;
	entry_obj->ent.entry->old_flags = entry_obj->ent.entry->flags;
	entry_obj->ent.entry->phar->is_modified = 1;
	entry_obj->ent.entry->is_modified = 1;

	/* hackish cache in php_stat needs to be cleared */
	/* if this code fails to work, check main/streams/streams.c, _php_stream_stat_path */
	if (BG(CurrentLStatFile)) {
		efree(BG(CurrentLStatFile));
	}

	if (BG(CurrentStatFile)) {
		efree(BG(CurrentStatFile));
	}

	BG(CurrentLStatFile) = NULL;
	BG(CurrentStatFile) = NULL;
	phar_flush(entry_obj->ent.entry->phar, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
}