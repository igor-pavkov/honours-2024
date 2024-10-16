PHP_FUNCTION(odbc_exec)
{
	zval *pv_conn;
	long pv_flags;
	char *query;
	int numArgs, query_len;
	odbc_result *result = NULL;
	odbc_connection *conn;
	RETCODE rc;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLUINTEGER      scrollopts;
#endif

	numArgs = ZEND_NUM_ARGS();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", &pv_conn, &query, &query_len, &pv_flags) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE2(conn, odbc_connection *, &pv_conn, -1, "ODBC-Link", le_conn, le_pconn);
		
	result = (odbc_result *)ecalloc(1, sizeof(odbc_result));

	rc = PHP_ODBC_SQLALLOCSTMT(conn->hdbc, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		efree(result);
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

	rc = SQLExecDirect(result->stmt, query, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO && rc != SQL_NO_DATA_FOUND) { 
		/* XXX FIXME we should really check out SQLSTATE with SQLError
		 * in case rc is SQL_SUCCESS_WITH_INFO here.
		 */
		odbc_sql_error(conn, result->stmt, "SQLExecDirect"); 
		SQLFreeStmt(result->stmt, SQL_DROP);
		efree(result);
		RETURN_FALSE;
	}

	SQLNumResultCols(result->stmt, &(result->numcols));
	
	/* For insert, update etc. cols == 0 */
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