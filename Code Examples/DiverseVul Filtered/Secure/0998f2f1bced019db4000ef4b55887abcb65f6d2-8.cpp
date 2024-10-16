int ldb_kv_index_transaction_commit(struct ldb_module *module)
{
	struct ldb_kv_private *ldb_kv = talloc_get_type(
	    ldb_module_get_private(module), struct ldb_kv_private);
	int ret;

	struct ldb_context *ldb = ldb_module_get_ctx(module);

	ldb_reset_err_string(ldb);

	if (ldb_kv->idxptr->itdb) {
		tdb_traverse(
		    ldb_kv->idxptr->itdb, ldb_kv_index_traverse_store, module);
		tdb_close(ldb_kv->idxptr->itdb);
	}

	ret = ldb_kv->idxptr->error;
	if (ret != LDB_SUCCESS) {
		if (!ldb_errstring(ldb)) {
			ldb_set_errstring(ldb, ldb_strerror(ret));
		}
		ldb_asprintf_errstring(ldb, "Failed to store index records in transaction commit: %s", ldb_errstring(ldb));
	}

	talloc_free(ldb_kv->idxptr);
	ldb_kv->idxptr = NULL;
	return ret;
}