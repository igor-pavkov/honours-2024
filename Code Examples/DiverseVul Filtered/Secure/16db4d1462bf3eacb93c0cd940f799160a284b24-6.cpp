PHP_FUNCTION(odbc_columns)
{
	zval *pv_conn;
	odbc_result *result = NULL;
	odbc_connection *conn;
	char *cat = NULL, *schema = NULL, *table = NULL, *column = NULL;
	int cat_len = 0, schema_len = 0, table_len = 0, column_len = 0;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|s!sss", &pv_conn, &cat, &cat_len, &schema, &schema_len,
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

	/* 
	 * Needed to make MS Access happy
	 */
	if (table && table_len && schema && schema_len == 0) {
		schema = NULL;
	}

	rc = SQLColumns(result->stmt, 
			cat, (SQLSMALLINT) cat_len,
			schema, (SQLSMALLINT) schema_len,
			table, (SQLSMALLINT) table_len,
			column, (SQLSMALLINT) column_len);

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLColumns");
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