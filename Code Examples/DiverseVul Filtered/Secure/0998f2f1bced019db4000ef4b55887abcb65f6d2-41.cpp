int ldb_kv_index_transaction_cancel(struct ldb_module *module)
{
	struct ldb_kv_private *ldb_kv = talloc_get_type(
	    ldb_module_get_private(module), struct ldb_kv_private);
	if (ldb_kv->idxptr && ldb_kv->idxptr->itdb) {
		tdb_close(ldb_kv->idxptr->itdb);
	}
	talloc_free(ldb_kv->idxptr);
	ldb_kv->idxptr = NULL;
	return LDB_SUCCESS;
}