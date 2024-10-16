PHP_FUNCTION(odbc_tables)
{
	zval *pv_conn;
	odbc_result   *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL, *type = NULL;
	int cat_len = 0, schema_len = 0, table_len = 0, type_len = 0;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|s!sss", &pv_conn, &cat, &cat_len, &schema, &schema_len, 
		&table, &table_len, &type, &type_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, &pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		efree(result);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_FALSE;
	}

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_FALSE;
	}

	/* This hack is needed to access table information in Access databases (fmk) */
	if (table && table_len && schema && schema_len == 0) {
		schema = NULL;
	}

	rc = SQLTables(result->stmt, 
			cat, SAFE_SQL_NTS(cat), 
			schema,	SAFE_SQL_NTS(schema), 
			table, SAFE_SQL_NTS(table),
			type, SAFE_SQL_NTS(type));

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLTables");
		efree(result);
		RETURN_FALSE;
	}

	result->numparams = 0;
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	result->conn_ptr = conn;
	result->fetched = 0;
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);
}