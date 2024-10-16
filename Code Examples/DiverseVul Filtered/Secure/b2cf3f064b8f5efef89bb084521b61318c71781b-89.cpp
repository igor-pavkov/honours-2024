PHP_METHOD(PharFileInfo, setMetadata)
{
	char *error;
	zval *metadata;

	PHAR_ENTRY_OBJECT();

	if (PHAR_G(readonly) && !entry_obj->ent.entry->phar->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (entry_obj->ent.entry->is_temp_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry is a temporary directory (not an actual entry in the archive), cannot set metadata"); \
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &metadata) == FAILURE) {
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
	if (entry_obj->ent.entry->metadata) {
		zval_ptr_dtor(&entry_obj->ent.entry->metadata);
		entry_obj->ent.entry->metadata = NULL;
	}

	MAKE_STD_ZVAL(entry_obj->ent.entry->metadata);
	ZVAL_ZVAL(entry_obj->ent.entry->metadata, metadata, 1, 0);

	entry_obj->ent.entry->is_modified = 1;
	entry_obj->ent.entry->phar->is_modified = 1;
	phar_flush(entry_obj->ent.entry->phar, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
}