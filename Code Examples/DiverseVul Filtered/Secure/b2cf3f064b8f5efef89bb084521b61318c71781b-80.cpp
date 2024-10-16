PHP_METHOD(Phar, setAlias)
{
	char *alias, *error, *oldalias;
	phar_archive_data **fd_ptr;
	int alias_len, oldalias_len, old_temp, readd = 0;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write out phar archive, phar is read-only");
		RETURN_FALSE;
	}

	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	if (phar_obj->arc.archive->is_data) {
		if (phar_obj->arc.archive->is_tar) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"A Phar alias cannot be set in a plain tar archive");
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"A Phar alias cannot be set in a plain zip archive");
		}
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &alias, &alias_len) == SUCCESS) {
		if (alias_len == phar_obj->arc.archive->alias_len && memcmp(phar_obj->arc.archive->alias, alias, alias_len) == 0) {
			RETURN_TRUE;
		}
		if (alias_len && SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void**)&fd_ptr)) {
			spprintf(&error, 0, "alias \"%s\" is already used for archive \"%s\" and cannot be used for other archives", alias, (*fd_ptr)->fname);
			if (SUCCESS == phar_free_alias(*fd_ptr, alias, alias_len TSRMLS_CC)) {
				efree(error);
				goto valid_alias;
			}
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
			RETURN_FALSE;
		}
		if (!phar_validate_alias(alias, alias_len)) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Invalid alias \"%s\" specified for phar \"%s\"", alias, phar_obj->arc.archive->fname);
			RETURN_FALSE;
		}
valid_alias:
		if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
			return;
		}
		if (phar_obj->arc.archive->alias_len && SUCCESS == zend_hash_find(&(PHAR_GLOBALS->phar_alias_map), phar_obj->arc.archive->alias, phar_obj->arc.archive->alias_len, (void**)&fd_ptr)) {
			zend_hash_del(&(PHAR_GLOBALS->phar_alias_map), phar_obj->arc.archive->alias, phar_obj->arc.archive->alias_len);
			readd = 1;
		}

		oldalias = phar_obj->arc.archive->alias;
		oldalias_len = phar_obj->arc.archive->alias_len;
		old_temp = phar_obj->arc.archive->is_temporary_alias;

		if (alias_len) {
			phar_obj->arc.archive->alias = estrndup(alias, alias_len);
		} else {
			phar_obj->arc.archive->alias = NULL;
		}

		phar_obj->arc.archive->alias_len = alias_len;
		phar_obj->arc.archive->is_temporary_alias = 0;
		phar_flush(phar_obj->arc.archive, NULL, 0, 0, &error TSRMLS_CC);

		if (error) {
			phar_obj->arc.archive->alias = oldalias;
			phar_obj->arc.archive->alias_len = oldalias_len;
			phar_obj->arc.archive->is_temporary_alias = old_temp;
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			if (readd) {
				zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), oldalias, oldalias_len, (void*)&(phar_obj->arc.archive), sizeof(phar_archive_data*), NULL);
			}
			efree(error);
			RETURN_FALSE;
		}

		zend_hash_add(&(PHAR_GLOBALS->phar_alias_map), alias, alias_len, (void*)&(phar_obj->arc.archive), sizeof(phar_archive_data*), NULL);

		if (oldalias) {
			efree(oldalias);
		}

		RETURN_TRUE;
	}

	RETURN_FALSE;
}