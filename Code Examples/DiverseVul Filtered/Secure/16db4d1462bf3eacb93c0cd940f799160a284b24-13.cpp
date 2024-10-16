PHP_MINIT_FUNCTION(odbc)
{
#ifdef SQLANY_BUG
	ODBC_SQL_CONN_T foobar;
	RETCODE rc;
#endif

	REGISTER_INI_ENTRIES();
	le_result = zend_register_list_destructors_ex(_free_odbc_result, NULL, "odbc result", module_number);
	le_conn = zend_register_list_destructors_ex(_close_odbc_conn, NULL, "odbc link", module_number);
	le_pconn = zend_register_list_destructors_ex(NULL, _close_odbc_pconn, "odbc link persistent", module_number);
	Z_TYPE(odbc_module_entry) = type;
	
	REGISTER_STRING_CONSTANT("ODBC_TYPE", PHP_ODBC_TYPE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ODBC_BINMODE_PASSTHRU", 0, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ODBC_BINMODE_RETURN", 1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ODBC_BINMODE_CONVERT", 2, CONST_CS | CONST_PERSISTENT);
	/* Define Constants for options
	   these Constants are defined in <sqlext.h>
	*/
	REGISTER_LONG_CONSTANT("SQL_ODBC_CURSORS", SQL_ODBC_CURSORS, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CUR_USE_DRIVER", SQL_CUR_USE_DRIVER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CUR_USE_IF_NEEDED", SQL_CUR_USE_IF_NEEDED, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CUR_USE_ODBC", SQL_CUR_USE_ODBC, CONST_PERSISTENT | CONST_CS);


	REGISTER_LONG_CONSTANT("SQL_CONCURRENCY", SQL_CONCURRENCY, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CONCUR_READ_ONLY", SQL_CONCUR_READ_ONLY, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CONCUR_LOCK", SQL_CONCUR_LOCK, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CONCUR_ROWVER", SQL_CONCUR_ROWVER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CONCUR_VALUES", SQL_CONCUR_VALUES, CONST_PERSISTENT | CONST_CS);

	REGISTER_LONG_CONSTANT("SQL_CURSOR_TYPE", SQL_CURSOR_TYPE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CURSOR_FORWARD_ONLY", SQL_CURSOR_FORWARD_ONLY, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CURSOR_KEYSET_DRIVEN", SQL_CURSOR_KEYSET_DRIVEN, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CURSOR_DYNAMIC", SQL_CURSOR_DYNAMIC, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_CURSOR_STATIC", SQL_CURSOR_STATIC, CONST_PERSISTENT | CONST_CS);
	
	REGISTER_LONG_CONSTANT("SQL_KEYSET_SIZE", SQL_KEYSET_SIZE, CONST_PERSISTENT | CONST_CS);

	/* these are for the Data Source type */
	REGISTER_LONG_CONSTANT("SQL_FETCH_FIRST", SQL_FETCH_FIRST, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_FETCH_NEXT", SQL_FETCH_NEXT, CONST_PERSISTENT | CONST_CS);

	/*
	 * register the standard data types
	 */
	REGISTER_LONG_CONSTANT("SQL_CHAR", SQL_CHAR, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_VARCHAR", SQL_VARCHAR, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_LONGVARCHAR", SQL_LONGVARCHAR, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_DECIMAL", SQL_DECIMAL, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_NUMERIC", SQL_NUMERIC, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_BIT", SQL_BIT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_TINYINT", SQL_TINYINT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_SMALLINT", SQL_SMALLINT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_INTEGER", SQL_INTEGER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_BIGINT", SQL_BIGINT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_REAL", SQL_REAL, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_FLOAT", SQL_FLOAT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_DOUBLE", SQL_DOUBLE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_BINARY", SQL_BINARY, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_VARBINARY", SQL_VARBINARY, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_LONGVARBINARY", SQL_LONGVARBINARY, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_DATE", SQL_DATE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_TIME", SQL_TIME, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_TIMESTAMP", SQL_TIMESTAMP, CONST_PERSISTENT | CONST_CS);
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
	REGISTER_LONG_CONSTANT("SQL_TYPE_DATE", SQL_TYPE_DATE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_TYPE_TIME", SQL_TYPE_TIME, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_TYPE_TIMESTAMP", SQL_TYPE_TIMESTAMP, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_WCHAR", SQL_WCHAR, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_WVARCHAR", SQL_WVARCHAR, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_WLONGVARCHAR", SQL_WLONGVARCHAR, CONST_PERSISTENT | CONST_CS);

	/*
	 * SQLSpecialColumns values
	 */
	REGISTER_LONG_CONSTANT("SQL_BEST_ROWID", SQL_BEST_ROWID, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_ROWVER", SQL_ROWVER, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_SCOPE_CURROW", SQL_SCOPE_CURROW, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_SCOPE_TRANSACTION", SQL_SCOPE_TRANSACTION, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_SCOPE_SESSION", SQL_SCOPE_SESSION, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_NO_NULLS", SQL_NO_NULLS, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_NULLABLE", SQL_NULLABLE, CONST_PERSISTENT | CONST_CS);

	/*
	 * SQLStatistics values
	 */
	REGISTER_LONG_CONSTANT("SQL_INDEX_UNIQUE", SQL_INDEX_UNIQUE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_INDEX_ALL", SQL_INDEX_ALL, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_ENSURE", SQL_ENSURE, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT("SQL_QUICK", SQL_QUICK, CONST_PERSISTENT | CONST_CS);
#endif

#if defined(HAVE_IBMDB2) && defined(_AIX)
	/* atexit() handler in the DB2/AIX library segfaults in PHP CLI */
	/* DB2NOEXITLIST env variable prevents DB2 from invoking atexit() */
	putenv("DB2NOEXITLIST=TRUE");
#endif

	return SUCCESS;
}