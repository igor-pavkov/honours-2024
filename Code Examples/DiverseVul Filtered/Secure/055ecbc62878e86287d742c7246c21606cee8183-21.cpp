static void php_sqlite3_func_step_callback(sqlite3_context *context, int argc,
	sqlite3_value **argv)
{
	struct pdo_sqlite_func *func = (struct pdo_sqlite_func*)sqlite3_user_data(context);
	TSRMLS_FETCH();

	do_callback(&func->astep, func->step, argc, argv, context, 1 TSRMLS_CC);
}