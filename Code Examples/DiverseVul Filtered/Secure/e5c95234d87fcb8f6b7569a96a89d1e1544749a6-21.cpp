PHP_METHOD(Phar, setAlias)
{
	char *alias, *error, *oldalias;
	phar_archive_data *fd_ptr;
	size_t alias_len, oldalias_len;
	int old_temp, readd = 0;

	PHAR_ARCHIVE_OBJECT();

	if (PHAR_G(readonly) && !phar_obj->archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
			"Cannot write out phar archive, phar is read-only");
		RETURN_FALSE;
	}

	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	if (phar_obj->archive->is_data) {
		if (phar_obj->archive->is_tar) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"A Phar alias cannot be set in a plain tar archive");
		} else {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"A Phar alias cannot be set in a plain zip archive");
		}
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &alias, &alias_len) == SUCCESS) {
		if (ZEND_SIZE_T_INT_OVFL(alias_len)) {
			RETURN_FALSE;
		}
		if (alias_len == (size_t)phar_obj->archive->alias_len && memcmp(phar_obj->archive->alias, alias, alias_len) == 0) {
			RETURN_TRUE;
		}
		if (alias_len && NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), alias, alias_len))) {
			spprintf(&error, 0, "alias \"%s\" is already used for archive \"%s\" and cannot be used for other archives", alias, fd_ptr->fname);
			if (SUCCESS == phar_free_alias(fd_ptr, alias, (int)alias_len)) {
				efree(error);
				goto valid_alias;
			}
			zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
			efree(error);
			RETURN_FALSE;
		}
		if (!phar_validate_alias(alias, (int)alias_len)) {
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0,
				"Invalid alias \"%s\" specified for phar \"%s\"", alias, phar_obj->archive->fname);
			RETURN_FALSE;
		}
valid_alias:
		if (phar_obj->archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->archive))) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "phar \"%s\" is persistent, unable to copy on write", phar_obj->archive->fname);
			return;
		}
		if (phar_obj->archive->alias_len && NULL != (fd_ptr = zend_hash_str_find_ptr(&(PHAR_G(phar_alias_map)), phar_obj->archive->alias, phar_obj->archive->alias_len))) {
			zend_hash_str_del(&(PHAR_G(phar_alias_map)), phar_obj->archive->alias, phar_obj->archive->alias_len);
			readd = 1;
		}

		oldalias = phar_obj->archive->alias;
		oldalias_len = phar_obj->archive->alias_len;
		old_temp = phar_obj->archive->is_temporary_alias;

		if (alias_len) {
			phar_obj->archive->alias = estrndup(alias, alias_len);
		} else {
			phar_obj->archive->alias = NULL;
		}

		phar_obj->archive->alias_len = (int)alias_len;
		phar_obj->archive->is_temporary_alias = 0;
		phar_flush(phar_obj->archive, NULL, 0, 0, &error);

		if (error) {
			phar_obj->archive->alias = oldalias;
			phar_obj->archive->alias_len = (int)oldalias_len;
			phar_obj->archive->is_temporary_alias = old_temp;
			zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
			if (readd) {
				zend_hash_str_add_ptr(&(PHAR_G(phar_alias_map)), oldalias, oldalias_len, phar_obj->archive);
			}
			efree(error);
			RETURN_FALSE;
		}

		zend_hash_str_add_ptr(&(PHAR_G(phar_alias_map)), alias, alias_len, phar_obj->archive);

		if (oldalias) {
			efree(oldalias);
		}

		RETURN_TRUE;
	}

	RETURN_FALSE;
}