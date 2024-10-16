static void pdo_sqlite_cleanup_callbacks(pdo_sqlite_db_handle *H TSRMLS_DC)
{
	struct pdo_sqlite_func *func;

	while (H->funcs) {
		func = H->funcs;
		H->funcs = func->next;

		if (H->db) {
			/* delete the function from the handle */
			sqlite3_create_function(H->db,
				func->funcname,
				func->argc,
				SQLITE_UTF8,
				func,
				NULL, NULL, NULL);
		}

		efree((char*)func->funcname);
		if (func->func) {
			zval_ptr_dtor(&func->func);
		}
		if (func->step) {
			zval_ptr_dtor(&func->step);
		}
		if (func->fini) {
			zval_ptr_dtor(&func->fini);
		}
		efree(func);
	}

	while (H->collations) {
		struct pdo_sqlite_collation *collation;
		collation = H->collations;
		H->collations = collation->next;

		if (H->db) {
			/* delete the collation from the handle */
			sqlite3_create_collation(H->db,
				collation->name,
				SQLITE_UTF8,
				collation,
				NULL);
		}

		efree((char*)collation->name);
		if (collation->callback) {
			zval_ptr_dtor(&collation->callback);
		}
		efree(collation);
	}
}