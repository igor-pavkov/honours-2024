static zend_object *phar_rename_archive(phar_archive_data **sphar, char *ext) /* {{{ */
{
	const char *oldname = NULL;
	phar_archive_data *phar = *sphar;
	char *oldpath = NULL;
	char *basename = NULL, *basepath = NULL;
	char *newname = NULL, *newpath = NULL;
	zval ret, arg1;
	zend_class_entry *ce;
	char *error = NULL;
	const char *pcr_error;
	int ext_len = ext ? strlen(ext) : 0;
	size_t new_len, oldname_len, phar_ext_len;
	phar_archive_data *pphar = NULL;
	php_stream_statbuf ssb;

	int phar_ext_list_len, i = 0;
	char *ext_pos = NULL;
	/* Array of PHAR extensions, Must be in order, starting with longest
	 * ending with the shortest. */
	char *phar_ext_list[] = {
		".phar.tar.bz2",
		".phar.tar.gz",
		".phar.php",
		".phar.bz2",
		".phar.zip",
		".phar.tar",
		".phar.gz",
		".tar.bz2",
		".tar.gz",
		".phar",
		".tar",
		".zip"
	};

	if (!ext) {
		if (phar->is_zip) {

			if (phar->is_data) {
				ext = "zip";
			} else {
				ext = "phar.zip";
			}

		} else if (phar->is_tar) {

			switch (phar->flags) {
				case PHAR_FILE_COMPRESSED_GZ:
					if (phar->is_data) {
						ext = "tar.gz";
					} else {
						ext = "phar.tar.gz";
					}
					break;
				case PHAR_FILE_COMPRESSED_BZ2:
					if (phar->is_data) {
						ext = "tar.bz2";
					} else {
						ext = "phar.tar.bz2";
					}
					break;
				default:
					if (phar->is_data) {
						ext = "tar";
					} else {
						ext = "phar.tar";
					}
			}
		} else {

			switch (phar->flags) {
				case PHAR_FILE_COMPRESSED_GZ:
					ext = "phar.gz";
					break;
				case PHAR_FILE_COMPRESSED_BZ2:
					ext = "phar.bz2";
					break;
				default:
					ext = "phar";
			}
		}
	} else if (phar_path_check(&ext, &ext_len, &pcr_error) > pcr_is_ok) {

		if (phar->is_data) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "data phar converted from \"%s\" has invalid extension %s", phar->fname, ext);
		} else {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "phar converted from \"%s\" has invalid extension %s", phar->fname, ext);
		}
		return NULL;
	}


	oldpath = estrndup(phar->fname, phar->fname_len);
	if ((oldname = zend_memrchr(phar->fname, '/', phar->fname_len))) {
		++oldname;
	} else {
		oldname = phar->fname;
	}

	oldname_len = strlen(oldname);
	/* Copy the old name to create base for the new name */
	basename = estrndup(oldname, oldname_len);

	phar_ext_list_len = sizeof(phar_ext_list)/sizeof(phar_ext_list[0]);
	/* Remove possible PHAR extensions */
	/* phar_ext_list must be in order of longest extension to shortest */
	for (i=0; i < phar_ext_list_len; i++) {
		phar_ext_len = strlen(phar_ext_list[i]);
		if (phar_ext_len && oldname_len > phar_ext_len) {
			/* Check if the basename strings ends with the extension */
			if (memcmp(phar_ext_list[i], basename + (oldname_len - phar_ext_len), phar_ext_len) == 0) {
				ext_pos = basename + (oldname_len - phar_ext_len);
				ext_pos[0] = '\0';
				break;
			}
		}
		ext_pos = NULL;
	}

	/* If no default PHAR extension found remove the last extension */
	if (!ext_pos) {
		ext_pos = strrchr(basename, '.');
		if (ext_pos) {
			ext_pos[0] = '\0';
		}
	}
	ext_pos = NULL;

	if (ext[0] == '.') {
		++ext;
	}
	/* Append extension to the basename */
	spprintf(&newname, 0, "%s.%s", basename, ext);
	efree(basename);

	basepath = estrndup(oldpath, (strlen(oldpath) - oldname_len));
	new_len = spprintf(&newpath, 0, "%s%s", basepath, newname);
	if (ZEND_SIZE_T_INT_OVFL(new_len)) {
		efree(oldpath);
		efree(basepath);
		efree(newpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "New name is too long");
		return NULL;
	}
	phar->fname_len = (int)new_len;
	phar->fname = newpath;
	phar->ext = newpath + phar->fname_len - strlen(ext) - 1;
	efree(basepath);
	efree(newname);

	if (PHAR_G(manifest_cached) && NULL != (pphar = zend_hash_str_find_ptr(&cached_phars, newpath, phar->fname_len))) {
		efree(oldpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Unable to add newly converted phar \"%s\" to the list of phars, new phar name is in phar.cache_list", phar->fname);
		return NULL;
	}

	if (NULL != (pphar = zend_hash_str_find_ptr(&(PHAR_G(phar_fname_map)), newpath, phar->fname_len))) {
		if (pphar->fname_len == phar->fname_len && !memcmp(pphar->fname, phar->fname, phar->fname_len)) {
			if (!zend_hash_num_elements(&phar->manifest)) {
				pphar->is_tar = phar->is_tar;
				pphar->is_zip = phar->is_zip;
				pphar->is_data = phar->is_data;
				pphar->flags = phar->flags;
				pphar->fp = phar->fp;
				phar->fp = NULL;
				phar_destroy_phar_data(phar);
				*sphar = NULL;
				phar = pphar;
				phar->refcount++;
				newpath = oldpath;
				goto its_ok;
			}
		}

		efree(oldpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Unable to add newly converted phar \"%s\" to the list of phars, a phar with that name already exists", phar->fname);
		return NULL;
	}
its_ok:
	if (SUCCESS == php_stream_stat_path(newpath, &ssb)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "phar \"%s\" exists and must be unlinked prior to conversion", newpath);
		efree(oldpath);
		return NULL;
	}
	if (!phar->is_data) {
		if (SUCCESS != phar_detect_phar_fname_ext(newpath, phar->fname_len, (const char **) &(phar->ext), &(phar->ext_len), 1, 1, 1)) {
			efree(oldpath);
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "phar \"%s\" has invalid extension %s", phar->fname, ext);
			return NULL;
		}

		if (phar->alias) {
			if (phar->is_temporary_alias) {
				phar->alias = NULL;
				phar->alias_len = 0;
			} else {
				phar->alias = estrndup(newpath, strlen(newpath));
				phar->alias_len = (int)strlen(newpath);
				phar->is_temporary_alias = 1;
				zend_hash_str_update_ptr(&(PHAR_G(phar_alias_map)), newpath, phar->fname_len, phar);
			}
		}

	} else {

		if (SUCCESS != phar_detect_phar_fname_ext(newpath, phar->fname_len, (const char **) &(phar->ext), &(phar->ext_len), 0, 1, 1)) {
			efree(oldpath);
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "data phar \"%s\" has invalid extension %s", phar->fname, ext);
			return NULL;
		}

		phar->alias = NULL;
		phar->alias_len = 0;
	}

	if ((!pphar || phar == pphar) && NULL == zend_hash_str_update_ptr(&(PHAR_G(phar_fname_map)), newpath, phar->fname_len, phar)) {
		efree(oldpath);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Unable to add newly converted phar \"%s\" to the list of phars", phar->fname);
		return NULL;
	}

	phar_flush(phar, 0, 0, 1, &error);

	if (error) {
		zend_hash_str_del(&(PHAR_G(phar_fname_map)), newpath, phar->fname_len);
		*sphar = NULL;
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "%s", error);
		efree(error);
		efree(oldpath);
		return NULL;
	}

	efree(oldpath);

	if (phar->is_data) {
		ce = phar_ce_data;
	} else {
		ce = phar_ce_archive;
	}

	ZVAL_NULL(&ret);
	if (SUCCESS != object_init_ex(&ret, ce)) {
		zval_dtor(&ret);
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Unable to instantiate phar object when converting archive \"%s\"", phar->fname);
		return NULL;
	}

	ZVAL_STRINGL(&arg1, phar->fname, phar->fname_len);

	zend_call_method_with_1_params(&ret, ce, &ce->constructor, "__construct", NULL, &arg1);
	zval_ptr_dtor(&arg1);
	return Z_OBJ(ret);
}