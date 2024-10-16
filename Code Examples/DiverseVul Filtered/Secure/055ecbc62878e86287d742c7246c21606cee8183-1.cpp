static int do_callback(struct pdo_sqlite_fci *fc, zval *cb,
		int argc, sqlite3_value **argv, sqlite3_context *context,
		int is_agg TSRMLS_DC)
{
	zval ***zargs = NULL;
	zval *retval = NULL;
	int i;
	int ret;
	int fake_argc;
	zval **agg_context = NULL;
	
	if (is_agg) {
		is_agg = 2;
	}
	
	fake_argc = argc + is_agg;
	
	fc->fci.size = sizeof(fc->fci);
	fc->fci.function_table = EG(function_table);
	fc->fci.function_name = cb;
	fc->fci.symbol_table = NULL;
	fc->fci.object_ptr = NULL;
	fc->fci.retval_ptr_ptr = &retval;
	fc->fci.param_count = fake_argc;
	
	/* build up the params */

	if (fake_argc) {
		zargs = (zval ***)safe_emalloc(fake_argc, sizeof(zval **), 0);
	}

	if (is_agg) {
		/* summon the aggregation context */
		agg_context = (zval**)sqlite3_aggregate_context(context, sizeof(zval*));
		if (!*agg_context) {
			MAKE_STD_ZVAL(*agg_context);
			ZVAL_NULL(*agg_context);
		}
		zargs[0] = agg_context;

		zargs[1] = emalloc(sizeof(zval*));
		MAKE_STD_ZVAL(*zargs[1]);
		ZVAL_LONG(*zargs[1], sqlite3_aggregate_count(context));
	}
	
	for (i = 0; i < argc; i++) {
		zargs[i + is_agg] = emalloc(sizeof(zval *));
		MAKE_STD_ZVAL(*zargs[i + is_agg]);

		/* get the value */
		switch (sqlite3_value_type(argv[i])) {
			case SQLITE_INTEGER:
				ZVAL_LONG(*zargs[i + is_agg], sqlite3_value_int(argv[i]));
				break;

			case SQLITE_FLOAT:
				ZVAL_DOUBLE(*zargs[i + is_agg], sqlite3_value_double(argv[i]));
				break;

			case SQLITE_NULL:
				ZVAL_NULL(*zargs[i + is_agg]);
				break;

			case SQLITE_BLOB:
			case SQLITE3_TEXT:
			default:
				ZVAL_STRINGL(*zargs[i + is_agg], (char*)sqlite3_value_text(argv[i]),
						sqlite3_value_bytes(argv[i]), 1);
				break;
		}
	}


	fc->fci.params = zargs;


	if ((ret = zend_call_function(&fc->fci, &fc->fcc TSRMLS_CC)) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the callback");
	}

	/* clean up the params */
	if (zargs) {
		for (i = is_agg; i < fake_argc; i++) {
			zval_ptr_dtor(zargs[i]);
			efree(zargs[i]);
		}
		if (is_agg) {
			zval_ptr_dtor(zargs[1]);
			efree(zargs[1]);
		}
		efree(zargs);
	}

	if (!is_agg || !argv) {
		/* only set the sqlite return value if we are a scalar function,
		 * or if we are finalizing an aggregate */
		if (retval) {
			switch (Z_TYPE_P(retval)) {
				case IS_LONG:
					sqlite3_result_int(context, Z_LVAL_P(retval));
					break;

				case IS_NULL:
					sqlite3_result_null(context);
					break;

				case IS_DOUBLE:
					sqlite3_result_double(context, Z_DVAL_P(retval));
					break;

				default:
					convert_to_string_ex(&retval);
					sqlite3_result_text(context, Z_STRVAL_P(retval),
						Z_STRLEN_P(retval), SQLITE_TRANSIENT);
					break;
			}
		} else {
			sqlite3_result_error(context, "failed to invoke callback", 0);
		}

		if (agg_context) {
			zval_ptr_dtor(agg_context);
		}
	} else {
		/* we're stepping in an aggregate; the return value goes into
		 * the context */
		if (agg_context) {
			zval_ptr_dtor(agg_context);
		}
		if (retval) {
			*agg_context = retval;
			retval = NULL;
		} else {
			*agg_context = NULL;
		}
	}

	if (retval) {
		zval_ptr_dtor(&retval);
	}

	return ret;
}