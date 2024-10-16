PHP_FUNCTION(odbc_result_all)
{
	char *buf = NULL;
	odbc_result *result;
	RETCODE rc;
	zval *pv_res;
	char *pv_format = NULL;
	int i, pv_format_len = 0;
	SQLSMALLINT sql_c_type;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLULEN crow;
	SQLUSMALLINT RowStatus[1];
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|s", &pv_res, &pv_format, &pv_format_len) == FAILURE) {
		return;
	}
				
	ZEND_FETCH_RESOURCE(result, odbc_result *, &pv_res, -1, "ODBC result", le_result);
	
	if (result->numcols == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
#ifdef HAVE_SQL_EXTENDED_FETCH
	if (result->fetch_abs)
		rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	else
#endif	
		rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		php_printf("<h2>No rows found</h2>\n");
		RETURN_LONG(0);
	}
	
	/* Start table tag */
	if (ZEND_NUM_ARGS() == 1) {
		php_printf("<table><tr>");
	} else {
		php_printf("<table %s ><tr>", pv_format);
	}
	
	for (i = 0; i < result->numcols; i++) {
		php_printf("<th>%s</th>", result->values[i].name);
	}

	php_printf("</tr>\n");

	while(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
		result->fetched++;
		php_printf("<tr>");
		for(i = 0; i < result->numcols; i++) {
			sql_c_type = SQL_C_CHAR;
			switch(result->values[i].coltype) {
				case SQL_BINARY:
				case SQL_VARBINARY:
				case SQL_LONGVARBINARY:
					if (result->binmode <= 0) {
						php_printf("<td>Not printable</td>");
						break;
					}
					if (result->binmode <= 1) sql_c_type = SQL_C_BINARY; 
				case SQL_LONGVARCHAR:
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
				case SQL_WLONGVARCHAR:
#endif
					if (IS_SQL_LONG(result->values[i].coltype) && 
						result->longreadlen <= 0) {
						php_printf("<td>Not printable</td>"); 
						break;
					}

					if (buf == NULL) {
						buf = emalloc(result->longreadlen);
					}

					rc = SQLGetData(result->stmt, (SQLUSMALLINT)(i + 1),sql_c_type, buf, result->longreadlen, &result->values[i].vallen);
 
					php_printf("<td>");

					if (rc == SQL_ERROR) {
						odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
						php_printf("</td></tr></table>");
						efree(buf);
						RETURN_FALSE;
					}
					if (rc == SQL_SUCCESS_WITH_INFO) {
						PHPWRITE(buf, result->longreadlen);
					} else if (result->values[i].vallen == SQL_NULL_DATA) {
						php_printf("<td>NULL</td>");
						break;
					} else {
						PHPWRITE(buf, result->values[i].vallen);
					}
					php_printf("</td>");
					break;
				default:
					if (result->values[i].vallen == SQL_NULL_DATA) {
						php_printf("<td>NULL</td>");
					} else {
						php_printf("<td>%s</td>", result->values[i].value);
					}
					break;
			}
		}
   		php_printf("</tr>\n");

#ifdef HAVE_SQL_EXTENDED_FETCH
		if (result->fetch_abs)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		else
#endif
			rc = SQLFetch(result->stmt);		
	}
	php_printf("</table>\n");
	if (buf) efree(buf);
	RETURN_LONG(result->fetched);
}