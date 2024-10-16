static int php_sqlite3_collation_callback(void *context,
	int string1_len, const void *string1,
	int string2_len, const void *string2)
{
	int ret;
	zval *zstring1, *zstring2;
	zval **zargs[2];
	zval *retval = NULL;
	struct pdo_sqlite_collation *collation = (struct pdo_sqlite_collation*) context;
	TSRMLS_FETCH();

	collation->fc.fci.size = sizeof(collation->fc.fci);
	collation->fc.fci.function_table = EG(function_table);
	collation->fc.fci.function_name = collation->callback;
	collation->fc.fci.symbol_table = NULL;
	collation->fc.fci.object_ptr = NULL;
	collation->fc.fci.retval_ptr_ptr = &retval;

	// Prepare the arguments.
	MAKE_STD_ZVAL(zstring1);
	ZVAL_STRINGL(zstring1, (char *) string1, string1_len, 1);
	zargs[0] = &zstring1;
	MAKE_STD_ZVAL(zstring2);
	ZVAL_STRINGL(zstring2, (char *) string2, string2_len, 1);
	zargs[1] = &zstring2;
	collation->fc.fci.param_count = 2;
	collation->fc.fci.params = zargs;

	if ((ret = zend_call_function(&collation->fc.fci, &collation->fc.fcc TSRMLS_CC)) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the callback");
	}
	else if (retval) {
		if (Z_TYPE_P(retval) != IS_LONG) {
			convert_to_long_ex(&retval);
		}
		ret = 0;
		if (Z_LVAL_P(retval) > 0) {
			ret = 1;
		}
		else if (Z_LVAL_P(retval) < 0) {
			ret = -1;
		}
		zval_ptr_dtor(&retval);
	}

	zval_ptr_dtor(zargs[0]);
	zval_ptr_dtor(zargs[1]);

	return ret;
}