PHP_METHOD(Phar, copy)
{
	char *oldfile, *newfile, *error;
	const char *pcr_error;
	int oldfile_len, newfile_len;
	phar_entry_info *oldentry, newentry = {0}, *temp;

	PHAR_ARCHIVE_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "pp", &oldfile, &oldfile_len, &newfile, &newfile_len) == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot copy \"%s\" to \"%s\", phar is read-only", oldfile, newfile);
		RETURN_FALSE;
	}

	if (oldfile_len >= sizeof(".phar")-1 && !memcmp(oldfile, ".phar", sizeof(".phar")-1)) {
		/* can't copy a meta file */
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"file \"%s\" cannot be copied to file \"%s\", cannot copy Phar meta-file in %s", oldfile, newfile, phar_obj->arc.archive->fname);
		RETURN_FALSE;
	}

	if (newfile_len >= sizeof(".phar")-1 && !memcmp(newfile, ".phar", sizeof(".phar")-1)) {
		/* can't copy a meta file */
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"file \"%s\" cannot be copied to file \"%s\", cannot copy to Phar meta-file in %s", oldfile, newfile, phar_obj->arc.archive->fname);
		RETURN_FALSE;
	}

	if (!zend_hash_exists(&phar_obj->arc.archive->manifest, oldfile, (uint) oldfile_len) || SUCCESS != zend_hash_find(&phar_obj->arc.archive->manifest, oldfile, (uint) oldfile_len, (void**)&oldentry) || oldentry->is_deleted) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"file \"%s\" cannot be copied to file \"%s\", file does not exist in %s", oldfile, newfile, phar_obj->arc.archive->fname);
		RETURN_FALSE;
	}

	if (zend_hash_exists(&phar_obj->arc.archive->manifest, newfile, (uint) newfile_len)) {
		if (SUCCESS == zend_hash_find(&phar_obj->arc.archive->manifest, newfile, (uint) newfile_len, (void**)&temp) || !temp->is_deleted) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"file \"%s\" cannot be copied to file \"%s\", file must not already exist in phar %s", oldfile, newfile, phar_obj->arc.archive->fname);
			RETURN_FALSE;
		}
	}

	if (phar_path_check(&newfile, &newfile_len, &pcr_error) > pcr_is_ok) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"file \"%s\" contains invalid characters %s, cannot be copied from \"%s\" in phar %s", newfile, pcr_error, oldfile, phar_obj->arc.archive->fname);
		RETURN_FALSE;
	}

	if (phar_obj->arc.archive->is_persistent) {
		if (FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
			return;
		}
		/* re-populate with copied-on-write entry */
		zend_hash_find(&phar_obj->arc.archive->manifest, oldfile, (uint) oldfile_len, (void**)&oldentry);
	}

	memcpy((void *) &newentry, oldentry, sizeof(phar_entry_info));

	if (newentry.metadata) {
		zval *t;

		t = newentry.metadata;
		ALLOC_ZVAL(newentry.metadata);
		*newentry.metadata = *t;
		zval_copy_ctor(newentry.metadata);
		Z_SET_REFCOUNT_P(newentry.metadata, 1);

		newentry.metadata_str.c = NULL;
		newentry.metadata_str.len = 0;
	}

	newentry.filename = estrndup(newfile, newfile_len);
	newentry.filename_len = newfile_len;
	newentry.fp_refcount = 0;

	if (oldentry->fp_type != PHAR_FP) {
		if (FAILURE == phar_copy_entry_fp(oldentry, &newentry, &error TSRMLS_CC)) {
			efree(newentry.filename);
			php_stream_close(newentry.fp);
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
			return;
		}
	}

	zend_hash_add(&oldentry->phar->manifest, newfile, newfile_len, (void*)&newentry, sizeof(phar_entry_info), NULL);
	phar_obj->arc.archive->is_modified = 1;
	phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}

	RETURN_TRUE;
}