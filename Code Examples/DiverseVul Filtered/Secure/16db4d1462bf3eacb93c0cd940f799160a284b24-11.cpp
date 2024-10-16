PHP_FUNCTION(odbc_result)
{
	char *field;
	int field_ind;
	SQLSMALLINT sql_c_type = SQL_C_CHAR;
	odbc_result *result;
	int i = 0;
	RETCODE rc;
	SQLLEN	fieldsize;
	zval *pv_res, **pv_field;
#ifdef HAVE_SQL_EXTENDED_FETCH
	SQLULEN crow;
	SQLUSMALLINT RowStatus[1];
#endif

	field_ind = -1;
	field = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rZ", &pv_res, &pv_field) == FAILURE) {
		return;
	}
	
	if (Z_TYPE_PP(pv_field) == IS_STRING) {
		field = Z_STRVAL_PP(pv_field);
	} else {
		convert_to_long_ex(pv_field);
		field_ind = Z_LVAL_PP(pv_field) - 1;
	}
	
	ZEND_FETCH_RESOURCE(result, odbc_result *, &pv_res, -1, "ODBC result", le_result);
	
	if ((result->numcols == 0)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	/* get field index if the field parameter was a string */
	if (field != NULL) {
		if (result->values == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Result set contains no data");
			RETURN_FALSE;
		}

		for(i = 0; i < result->numcols; i++) {
			if (!strcasecmp(result->values[i].name, field)) {
				field_ind = i;
				break;
			}
		}

		if (field_ind < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Field %s not found", field);
			RETURN_FALSE;
		}
	} else {
		/* check for limits of field_ind if the field parameter was an int */
		if (field_ind >= result->numcols || field_ind < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Field index is larger than the number of fields");
			RETURN_FALSE;
		}
	}

	if (result->fetched == 0) {
		/* User forgot to call odbc_fetch_row(), or wants to reload the results, do it now */
#ifdef HAVE_SQL_EXTENDED_FETCH
		if (result->fetch_abs)
			rc = SQLExtendedFetch(result->stmt, SQL_FETCH_NEXT, 1, &crow,RowStatus);
		else
#endif
			rc = SQLFetch(result->stmt);

		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			RETURN_FALSE;
		}
		
		result->fetched++;
	}

	switch(result->values[field_ind].coltype) {
		case SQL_BINARY:
		case SQL_VARBINARY:
		case SQL_LONGVARBINARY:
			if (result->binmode <= 1) {
				sql_c_type = SQL_C_BINARY;
			}
			if (result->binmode <= 0) {
				break; 
			}
		case SQL_LONGVARCHAR:
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
		case SQL_WLONGVARCHAR:
#endif
			if (IS_SQL_LONG(result->values[field_ind].coltype)) {
				if (result->longreadlen <= 0) {
				   break;
				} else {
				   fieldsize = result->longreadlen;
				}
			} else {
			   PHP_ODBC_SQLCOLATTRIBUTE(result->stmt, (SQLUSMALLINT)(field_ind + 1), 
					   			(SQLUSMALLINT)((sql_c_type == SQL_C_BINARY) ? SQL_COLUMN_LENGTH :
					   			SQL_COLUMN_DISPLAY_SIZE),
					   			NULL, 0, NULL, &fieldsize);
			}
			/* For char data, the length of the returned string will be longreadlen - 1 */
			fieldsize = (result->longreadlen <= 0) ? 4096 : result->longreadlen;
			field = emalloc(fieldsize);

		/* SQLGetData will truncate CHAR data to fieldsize - 1 bytes and append \0.
		 * For binary data it is truncated to fieldsize bytes. 
		 */
			rc = SQLGetData(result->stmt, (SQLUSMALLINT)(field_ind + 1), sql_c_type,
							field, fieldsize, &result->values[field_ind].vallen);

			if (rc == SQL_ERROR) {
				odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
				efree(field);
				RETURN_FALSE;
			}

			if (result->values[field_ind].vallen == SQL_NULL_DATA) {
				efree(field);
				RETURN_NULL();
			} else if (rc == SQL_NO_DATA_FOUND) {
				efree(field);
				RETURN_FALSE;
			}
			/* Reduce fieldlen by 1 if we have char data. One day we might 
			   have binary strings... */
			if ((result->values[field_ind].coltype == SQL_LONGVARCHAR)
#if defined(ODBCVER) && (ODBCVER >= 0x0300)
			    || (result->values[field_ind].coltype == SQL_WLONGVARCHAR)
#endif
			) {
				fieldsize -= 1;
			}
			/* Don't duplicate result, saves one emalloc.
			   For SQL_SUCCESS, the length is in vallen.
			 */
			RETURN_STRINGL(field, (rc == SQL_SUCCESS_WITH_INFO) ? fieldsize : result->values[field_ind].vallen, 0);
			break;
			
		default:
			if (result->values[field_ind].vallen == SQL_NULL_DATA) {
				RETURN_NULL();
			} else {
				RETURN_STRINGL(result->values[field_ind].value, result->values[field_ind].vallen, 1);
			}
			break;
	}

/* If we come here, output unbound LONG and/or BINARY column data to the client */
	
	/* We emalloc 1 byte more for SQL_C_CHAR (trailing \0) */
	fieldsize = (sql_c_type == SQL_C_CHAR) ? 4096 : 4095;
	field = emalloc(fieldsize);
	
	/* Call SQLGetData() until SQL_SUCCESS is returned */
	while(1) {
		rc = SQLGetData(result->stmt, (SQLUSMALLINT)(field_ind + 1),sql_c_type, field, fieldsize, &result->values[field_ind].vallen);

		if (rc == SQL_ERROR) {
			odbc_sql_error(result->conn_ptr, result->stmt, "SQLGetData");
			efree(field);
			RETURN_FALSE;
		}
		
		if (result->values[field_ind].vallen == SQL_NULL_DATA) {
			efree(field);
			RETURN_NULL();
		}
		/* chop the trailing \0 by outputing only 4095 bytes */
		PHPWRITE(field,(rc == SQL_SUCCESS_WITH_INFO) ? 4095 : result->values[field_ind].vallen);

		if (rc == SQL_SUCCESS) { /* no more data avail */
			efree(field);
			RETURN_TRUE;
		}
	}
	RETURN_TRUE;
}