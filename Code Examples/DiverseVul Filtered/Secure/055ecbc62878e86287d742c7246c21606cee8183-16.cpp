static void php_sqlite3_func_callback(sqlite3_context *context, int argc,
	sqlite3_value **argv)
{
	struct pdo_sqlite_func *func = (struct pdo_sqlite_func*)sqlite3_user_data(context);
	TSRMLS_FETCH();

	do_callback(&func->afunc, func->func, argc, argv, context, 0 TSRMLS_CC);
}