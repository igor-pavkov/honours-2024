static PHP_METHOD(SQLite, sqliteCreateCollation)
{
	struct pdo_sqlite_collation *collation;
	zval *callback;
	char *collation_name;
	int collation_name_len;
	char *cbname = NULL;
	pdo_dbh_t *dbh;
	pdo_sqlite_db_handle *H;
	int ret;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz",
		&collation_name, &collation_name_len, &callback)) {
		RETURN_FALSE;
	}

	dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	PDO_CONSTRUCT_CHECK;

	if (!zend_is_callable(callback, 0, &cbname TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "function '%s' is not callable", cbname);
		efree(cbname);
		RETURN_FALSE;
	}
	efree(cbname);

	H = (pdo_sqlite_db_handle *)dbh->driver_data;

	collation = (struct pdo_sqlite_collation*)ecalloc(1, sizeof(*collation));

	ret = sqlite3_create_collation(H->db, collation_name, SQLITE_UTF8, collation, php_sqlite3_collation_callback);
	if (ret == SQLITE_OK) {
		collation->name = estrdup(collation_name);

		MAKE_STD_ZVAL(collation->callback);
		MAKE_COPY_ZVAL(&callback, collation->callback);

		collation->next = H->collations;
		H->collations = collation;

		RETURN_TRUE;
	}

	efree(collation);
	RETURN_FALSE;
}