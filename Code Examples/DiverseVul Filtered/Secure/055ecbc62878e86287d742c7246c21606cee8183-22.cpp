static void php_sqlite3_func_final_callback(sqlite3_context *context)
{
	struct pdo_sqlite_func *func = (struct pdo_sqlite_func*)sqlite3_user_data(context);
	TSRMLS_FETCH();

	do_callback(&func->afini, func->fini, 0, NULL, context, 1 TSRMLS_CC);
}