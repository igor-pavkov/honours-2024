static HashTable *spl_filesystem_object_get_debug_info(zval *object, int *is_temp) /* {{{ */
{
	spl_filesystem_object *intern = Z_SPLFILESYSTEM_P(object);
	zval tmp;
	HashTable *rv;
	zend_string *pnstr;
	char *path;
	size_t  path_len;
	char stmp[2];

	*is_temp = 1;

	if (!intern->std.properties) {
		rebuild_object_properties(&intern->std);
	}

	rv = zend_array_dup(intern->std.properties);

	pnstr = spl_gen_private_prop_name(spl_ce_SplFileInfo, "pathName", sizeof("pathName")-1);
	path = spl_filesystem_object_get_pathname(intern, &path_len);
	ZVAL_STRINGL(&tmp, path, path_len);
	zend_symtable_update(rv, pnstr, &tmp);
	zend_string_release(pnstr);

	if (intern->file_name) {
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileInfo, "fileName", sizeof("fileName")-1);
		spl_filesystem_object_get_path(intern, &path_len);

		if (path_len && path_len < intern->file_name_len) {
			ZVAL_STRINGL(&tmp, intern->file_name + path_len + 1, intern->file_name_len - (path_len + 1));
		} else {
			ZVAL_STRINGL(&tmp, intern->file_name, intern->file_name_len);
		}
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release(pnstr);
	}
	if (intern->type == SPL_FS_DIR) {
#ifdef HAVE_GLOB
		pnstr = spl_gen_private_prop_name(spl_ce_DirectoryIterator, "glob", sizeof("glob")-1);
		if (php_stream_is(intern->u.dir.dirp ,&php_glob_stream_ops)) {
			ZVAL_STRINGL(&tmp, intern->_path, intern->_path_len);
		} else {
			ZVAL_FALSE(&tmp);
		}
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release(pnstr);
#endif
		pnstr = spl_gen_private_prop_name(spl_ce_RecursiveDirectoryIterator, "subPathName", sizeof("subPathName")-1);
		if (intern->u.dir.sub_path) {
			ZVAL_STRINGL(&tmp, intern->u.dir.sub_path, intern->u.dir.sub_path_len);
		} else {
			ZVAL_EMPTY_STRING(&tmp);
		}
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release(pnstr);
	}
	if (intern->type == SPL_FS_FILE) {
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "openMode", sizeof("openMode")-1);
		ZVAL_STRINGL(&tmp, intern->u.file.open_mode, intern->u.file.open_mode_len);
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release(pnstr);
		stmp[1] = '\0';
		stmp[0] = intern->u.file.delimiter;
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "delimiter", sizeof("delimiter")-1);
		ZVAL_STRINGL(&tmp, stmp, 1);
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release(pnstr);
		stmp[0] = intern->u.file.enclosure;
		pnstr = spl_gen_private_prop_name(spl_ce_SplFileObject, "enclosure", sizeof("enclosure")-1);
		ZVAL_STRINGL(&tmp, stmp, 1);
		zend_symtable_update(rv, pnstr, &tmp);
		zend_string_release(pnstr);
	}

	return rv;
}