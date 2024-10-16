PHP_FUNCTION(odbc_prepare)
{
	zval *pv_conn;
	char *query;
	int query_len;
	odbc_result *result = NULL;
	odbc_connection *conn;
	RETCODE rc;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLUINTEGER      scrollopts;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &pv_conn, &query, &query_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, &pv_conn, -1, "ODBC-Link", le_conn, le_pconn);

	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));
	
	result->numparams = 0;
	
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

#ifdef HAVE_SQL_EXTENDED_FETCH
	/* Solid doesn't have ExtendedFetch, if DriverManager is used, get Info,
	   whether Driver supports ExtendedFetch */
	rc = SQLGetInfo(conn->hdbc, SQL_FETCH_DIRECTION, (void *) &scrollopts, sizeof(scrollopts), NULL);
	if (rc == SQL_SUCCESS) {
		if ((result->fetch_abs = (scrollopts & SQL_FD_FETCH_ABSOLUTE))) {
			/* Try to set CURSOR_TYPE to dynamic. Driver will replace this with other
			   type if not possible.
			*/
			SQLSetStmtOption(result->stmt, SQL_CURSOR_TYPE, ODBCG(default_cursortype));
		}
	} else {
		result->fetch_abs = 0;
	}
#endif

	rc = SQLPrepare(result->stmt, query, SQL_NTS);
	switch (rc) {
		case SQL_SUCCESS:
			break;
		case SQL_SUCCESS_WITH_INFO:
			odbc_sql_error(conn, result->stmt, "SQLPrepare");
			break;
		default:
			odbc_sql_error(conn, result->stmt, "SQLPrepare");
			RETURN_FALSE;
	}
	
	SQLNumParams(result->stmt, &(result->numparams));
	SQLNumResultCols(result->stmt, &(result->numcols));

	if (result->numcols > 0) {
		if (!odbc_bindcols(result TSRMLS_CC)) {
			efree(result);
			RETURN_FALSE;
		}
	} else {
		result->values = NULL;
	}
	zend_list_addref(conn->id);
	result->conn_ptr = conn;
	result->fetched = 0;
	ZEND_REGISTER_RESOURCE(return_value, result, le_result);  
}