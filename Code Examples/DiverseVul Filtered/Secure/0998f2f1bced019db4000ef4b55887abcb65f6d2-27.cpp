int ldb_kv_index_transaction_start(struct ldb_module *module)
{
	struct ldb_kv_private *ldb_kv = talloc_get_type(
	    ldb_module_get_private(module), struct ldb_kv_private);
	ldb_kv->idxptr = talloc_zero(ldb_kv, struct ldb_kv_idxptr);
	if (ldb_kv->idxptr == NULL) {
		return ldb_oom(ldb_module_get_ctx(module));
	}

	return LDB_SUCCESS;
}