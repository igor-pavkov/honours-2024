static int pdo_sqlite_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC) /* {{{ */
{
	pdo_sqlite_db_handle *H;
	int i, ret = 0;
	long timeout = 60;
	char *filename;

	H = pecalloc(1, sizeof(pdo_sqlite_db_handle), dbh->is_persistent);
	
	H->einfo.errcode = 0;
	H->einfo.errmsg = NULL;
	dbh->driver_data = H;

	filename = make_filename_safe(dbh->data_source TSRMLS_CC);

	if (!filename) {
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC,
			"safe_mode/open_basedir prohibits opening %s",
			dbh->data_source);
		goto cleanup;
	}

	i = sqlite3_open(filename, &H->db);
	efree(filename);

	if (i != SQLITE_OK) {
		pdo_sqlite_error(dbh);
		goto cleanup;
	}

	if (PG(safe_mode) || (PG(open_basedir) && *PG(open_basedir))) {
		sqlite3_set_authorizer(H->db, authorizer, NULL);
	}

	if (driver_options) {
		timeout = pdo_attr_lval(driver_options, PDO_ATTR_TIMEOUT, timeout TSRMLS_CC);
	}
	sqlite3_busy_timeout(H->db, timeout * 1000);

	dbh->alloc_own_columns = 1;
	dbh->max_escaped_char_length = 2;

	ret = 1;
	
cleanup:
	dbh->methods = &sqlite_methods;
	
	return ret;
}