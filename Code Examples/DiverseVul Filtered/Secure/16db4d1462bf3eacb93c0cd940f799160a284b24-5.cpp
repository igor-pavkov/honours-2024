PHP_FUNCTION(odbc_field_type)
{
	odbc_result	*result;
	char    	tmp[32];
	SQLSMALLINT	tmplen;
	zval		*pv_res;
	long		pv_num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &pv_res, &pv_num) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(result, odbc_result *, &pv_res, -1, "ODBC result", le_result);

	if (result->numcols == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (pv_num > result->numcols) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}

	if (pv_num < 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Field numbering starts at 1");
		RETURN_FALSE;
	}

	PHP_ODBC_SQLCOLATTRIBUTE(result->stmt, (SQLUSMALLINT)pv_num, SQL_COLUMN_TYPE_NAME, tmp, 31, &tmplen, NULL);
	RETURN_STRING(tmp,1)
}