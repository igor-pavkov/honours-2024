static void php_odbc_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int result_type)
{
	int i;
	odbc_result *result;
	RETCODE rc;
	SQLSMALLINT sql_c_type;
	char *buf = NULL;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLULEN crow;
	SQLUSMALLINT RowStatus[1];
	SQLLEN rownum;
	zval *pv_res, *tmp;
	long pv_row = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &pv_res, &pv_row) == FAILURE) {
		return;
	}
	
	rownum = pv_row;
#else
	zval *pv_res, *tmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &pv_res) == FAILURE) {
		return;
	}
#endif

	ZEND_FETCH_RESOURCE(result, odbc_result *, &pv_res, -1, "ODBC result", le_result);

	if (result->numcols == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

#ifdef HAVE_SQL_EXTENDED_FETCH
	if (result->fetch_abs) {
		if (rownum > 0) {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
		} else {
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		}
	} else
#endif
	rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	
#ifdef HAVE_SQL_EXTENDED_FETCH
	if (rownum > 0 && result->fetch_abs)
		result->fetched = rownum;
	else
#endif
		result->fetched++;

	for(i = 0; i < result->numcols; i++) {
		ALLOC_INIT_ZVAL(tmp);
		Z_TYPE_P(tmp) = IS_STRING;
		Z_STRLEN_P(tmp) = 0;
		sql_c_type = SQL_C_CHAR;

		switch(result->values[i].coltype) {
			case SQL_BINARY:
			case SQL_VARBINARY:
			case SQL_LONGVARBINARY:
				if (result->binmode <= 0) {
					Z_STRVAL_P(tmp) = STR_EMPTY_ALLOC();
					break;
				}
				if (result->binmode == 1) {
					sql_c_type = SQL_C_BINARY;
				}
			case SQL_LONGVARCHAR:
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
			case SQL_WLONGVARCHAR:
#endif
				if (IS_SQL_LONG(result->values[i].coltype) && result->longreadlen <= 0) {
					Z_STRVAL_P(tmp) = STR_EMPTY_ALLOC();
					break;
				}				
				if (buf == NULL) {
					buf = emalloc(result->longreadlen + 1);
				}
				
				rc = SQLGetData(result->stmt, (SQLUSMALLINT)(i + 1), sql_c_type, buf, result->longreadlen + 1, &result->values[i].vallen);

				if (rc == SQL_ERROR) {
					odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
					efree(buf);
					RETURN_FALSE;
				}

				if (rc == SQL_SUCCESS_WITH_INFO) {
					Z_STRLEN_P(tmp) = result->longreadlen;
				} else if (result->values[i].vallen == SQL_NULL_DATA) {
					ZVAL_NULL(tmp);
					break;
				} else {
					Z_STRLEN_P(tmp) = result->values[i].vallen;
				}
				Z_STRVAL_P(tmp) = estrndup(buf, Z_STRLEN_P(tmp));
				break;

			default:
				if (result->values[i].vallen == SQL_NULL_DATA) {
					ZVAL_NULL(tmp);
					break;
				}
				Z_STRLEN_P(tmp) = result->values[i].vallen;
				Z_STRVAL_P(tmp) = estrndup(result->values[i].value,Z_STRLEN_P(tmp));
				break;
		}

		if (result_type & ODBC_NUM) {
			zend_hash_index_update(Z_ARRVAL_P(return_value), i, &tmp, sizeof(zval *), NULL);
		} else {
			if (!*(result->values[i].name) && Z_TYPE_P(tmp) == IS_STRING) {
				zend_hash_update(Z_ARRVAL_P(return_value), Z_STRVAL_P(tmp),	Z_STRLEN_P(tmp)+1, &tmp, sizeof(zval *), NULL);
			} else {
				zend_hash_update(Z_ARRVAL_P(return_value), result->values[i].name, strlen(result->values[i].name)+1, &tmp, sizeof(zval *), NULL);
			}
		}
	}
	if (buf) {
		efree(buf);
	}
}