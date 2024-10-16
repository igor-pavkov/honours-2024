PHP_FUNCTION(odbc_foreignkeys)
{
	zval *pv_conn;
	odbc_result *result = NULL;
	odbc_connection *conn;
	char *pcat = NULL, *pschema, *ptable, *fcat, *fschema, *ftable;
	int pcat_len = 0, pschema_len, ptable_len, fcat_len, fschema_len, ftable_len;
	RETCODE rc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs!sssss", &pv_conn, &pcat, &pcat_len, &pschema, &pschema_len, 
		&ptable, &ptable_len, &fcat, &fcat_len, &fschema, &fschema_len, &ftable, &ftable_len) == FAILURE) {
		return;
	}

#if defined(HAVE_DBMAKER) || defined(HAVE_IBMDB2)
#define EMPTY_TO_NULL(xstr) \
	if ((int)strlen((xstr)) == 0) (xstr) = NULL

		EMPTY_TO_NULL(pcat);
		EMPTY_TO_NULL(pschema);
		EMPTY_TO_NULL(ptable);
		EMPTY_TO_NULL(fcat);
		EMPTY_TO_NULL(fschema);
		EMPTY_TO_NULL(ftable);
#endif

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

	rc = SQLForeignKeys(result->stmt, 
			pcat, SAFE_SQL_NTS(pcat), 
			pschema, SAFE_SQL_NTS(pschema), 
			ptable, SAFE_SQL_NTS(ptable), 
			fcat, SAFE_SQL_NTS(fcat), 
			fschema, SAFE_SQL_NTS(fschema), 
			ftable, SAFE_SQL_NTS(ftable) );

	if (rc == SQL_ERROR) {
		odbc_sql_error(conn, SQL_NULL_HSTMT, "SQLForeignKeys");
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