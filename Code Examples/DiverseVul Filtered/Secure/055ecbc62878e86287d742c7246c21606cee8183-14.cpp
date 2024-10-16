static PHP_METHOD(SQLite, sqliteCreateAggregate)
{
	struct pdo_sqlite_func *func;
	zval *step_callback, *fini_callback;
	char *func_name;
	int func_name_len;
	long argc = -1;
	char *cbname = NULL;
	pdo_dbh_t *dbh;
	pdo_sqlite_db_handle *H;
	int ret;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szz|l",
			&func_name, &func_name_len, &step_callback, &fini_callback, &argc)) {
		RETURN_FALSE;
	}
	
	dbh = zend_object_store_get_object(getThis() TSRMLS_CC);
	PDO_CONSTRUCT_CHECK;

	if (!zend_is_callable(step_callback, 0, &cbname TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "function '%s' is not callable", cbname);
		efree(cbname);
		RETURN_FALSE;
	}
	efree(cbname);
	if (!zend_is_callable(fini_callback, 0, &cbname TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "function '%s' is not callable", cbname);
		efree(cbname);
		RETURN_FALSE;
	}
	efree(cbname);
	
	H = (pdo_sqlite_db_handle *)dbh->driver_data;

	func = (struct pdo_sqlite_func*)ecalloc(1, sizeof(*func));

	ret = sqlite3_create_function(H->db, func_name, argc, SQLITE_UTF8,
			func, NULL, php_sqlite3_func_step_callback, php_sqlite3_func_final_callback);
	if (ret == SQLITE_OK) {
		func->funcname = estrdup(func_name);
		
		MAKE_STD_ZVAL(func->step);
		MAKE_COPY_ZVAL(&step_callback, func->step);

		MAKE_STD_ZVAL(func->fini);
		MAKE_COPY_ZVAL(&fini_callback, func->fini);
		
		func->argc = argc;

		func->next = H->funcs;
		H->funcs = func;

		RETURN_TRUE;
	}

	efree(func);
	RETURN_FALSE;
}