PHP_FUNCTION(odbc_columnprivileges)
{
	zval *pv_conn;
	odbc_result *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema, *table, *column;
	int cat_len = 0, schema_len, table_len, column_len;
	RETCODE rc;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs!sss", &pv_conn, &cat, &cat_len, &schema, &schema_len,
		&table, &table_len, &column, &column_len) == FAILURE) {
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

	rc = SQLColumnPrivileges(result->stmt, 
			cat, SAFE_SQL_NTS(cat),
			schema, SAFE_SQL_NTS(schema),
			table, SAFE_SQL_NTS(table),
			column, SAFE_SQL_NTS(column));

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLColumnPrivileges");
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