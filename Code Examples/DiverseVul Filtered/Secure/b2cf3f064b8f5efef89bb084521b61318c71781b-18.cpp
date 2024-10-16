PHP_METHOD(PharFileInfo, compress)
{
	long method;
	char *error;
	PHAR_ENTRY_OBJECT();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &method) == FAILURE) {
		return;
	}

	if (entry_obj->ent.entry->is_tar) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot compress with Gzip compression, not possible with tar-based phar archives");
		return;
	}

	if (entry_obj->ent.entry->is_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry is a directory, cannot set compression"); \
		return;
	}

	if (PHAR_G(readonly) && !entry_obj->ent.entry->phar->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar is readonly, cannot change compression");
		return;
	}

	if (entry_obj->ent.entry->is_deleted) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot compress deleted file");
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
	switch (method) {
		case PHAR_ENT_COMPRESSED_GZ:
			if (entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_GZ) {
				RETURN_TRUE;
			}

			if ((entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_BZ2) != 0) {
				if (!PHAR_G(has_bz2)) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
						"Cannot compress with gzip compression, file is already compressed with bzip2 compression and bz2 extension is not enabled, cannot decompress");
					return;
				}

				/* decompress this file indirectly */
				if (SUCCESS != phar_open_entry_fp(entry_obj->ent.entry, &error, 1 TSRMLS_CC)) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
						"Phar error: Cannot decompress bzip2-compressed file \"%s\" in phar \"%s\" in order to compress with gzip: %s", entry_obj->ent.entry->filename, entry_obj->ent.entry->phar->fname, error);
					efree(error);
					return;
				}
			}

			if (!PHAR_G(has_zlib)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress with gzip compression, zlib extension is not enabled");
				return;
			}

			entry_obj->ent.entry->old_flags = entry_obj->ent.entry->flags;
			entry_obj->ent.entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
			entry_obj->ent.entry->flags |= PHAR_ENT_COMPRESSED_GZ;
			break;
		case PHAR_ENT_COMPRESSED_BZ2:
			if (entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_BZ2) {
				RETURN_TRUE;
			}

			if ((entry_obj->ent.entry->flags & PHAR_ENT_COMPRESSED_GZ) != 0) {
				if (!PHAR_G(has_zlib)) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
						"Cannot compress with bzip2 compression, file is already compressed with gzip compression and zlib extension is not enabled, cannot decompress");
					return;
				}

				/* decompress this file indirectly */
				if (SUCCESS != phar_open_entry_fp(entry_obj->ent.entry, &error, 1 TSRMLS_CC)) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
						"Phar error: Cannot decompress gzip-compressed file \"%s\" in phar \"%s\" in order to compress with bzip2: %s", entry_obj->ent.entry->filename, entry_obj->ent.entry->phar->fname, error);
					efree(error);
					return;
				}
			}

			if (!PHAR_G(has_bz2)) {
				zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
					"Cannot compress with bzip2 compression, bz2 extension is not enabled");
				return;
			}
			entry_obj->ent.entry->old_flags = entry_obj->ent.entry->flags;
			entry_obj->ent.entry->flags &= ~PHAR_ENT_COMPRESSION_MASK;
			entry_obj->ent.entry->flags |= PHAR_ENT_COMPRESSED_BZ2;
			break;
		default:
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
				"Unknown compression type specified"); \
	}

	entry_obj->ent.entry->phar->is_modified = 1;
	entry_obj->ent.entry->is_modified = 1;
	phar_flush(entry_obj->ent.entry->phar, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}

	RETURN_TRUE;
}