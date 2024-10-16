void odbc_column_lengths(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	odbc_result *result;
#if defined(HAVE_SOLID) || defined(HAVE_SOLID_30)
	/* this seems to be necessary for Solid2.3 ( tested by 
	 * tammy@synchronis.com) and Solid 3.0 (tested by eric@terra.telemediair.nl)
	 * Solid does not seem to declare a SQLINTEGER, but it does declare a
	 * SQL_INTEGER which does not work (despite being the same type as a SDWORD.
	 * Solid 3.5 does not have this issue.
	 */
	SDWORD len;
#else
	SQLLEN len;
#endif
	zval *pv_res;
	long pv_num;

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

	PHP_ODBC_SQLCOLATTRIBUTE(result->stmt, (SQLUSMALLINT)pv_num, (SQLUSMALLINT) (type?SQL_COLUMN_SCALE:SQL_COLUMN_PRECISION), NULL, 0, NULL, &len);

	RETURN_LONG(len);
}