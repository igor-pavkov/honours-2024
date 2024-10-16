static int ldb_kv_index_traverse_store(struct tdb_context *tdb,
				       TDB_DATA key,
				       TDB_DATA data,
				       void *state)
{
	struct ldb_module *module = state;
	struct ldb_kv_private *ldb_kv = talloc_get_type(
	    ldb_module_get_private(module), struct ldb_kv_private);
	struct ldb_dn *dn;
	struct ldb_context *ldb = ldb_module_get_ctx(module);
	struct ldb_val v;
	struct dn_list *list;

	list = ldb_kv_index_idxptr(module, data, true);
	if (list == NULL) {
		ldb_kv->idxptr->error = LDB_ERR_OPERATIONS_ERROR;
		return -1;
	}

	v.data = key.dptr;
	v.length = strnlen((char *)key.dptr, key.dsize);

	dn = ldb_dn_from_ldb_val(module, ldb, &v);
	if (dn == NULL) {
		ldb_asprintf_errstring(ldb, "Failed to parse index key %*.*s as an LDB DN", (int)v.length, (int)v.length, (const char *)v.data);
		ldb_kv->idxptr->error = LDB_ERR_OPERATIONS_ERROR;
		return -1;
	}

	ldb_kv->idxptr->error =
	    ldb_kv_dn_list_store_full(module, ldb_kv, dn, list);
	talloc_free(dn);
	if (ldb_kv->idxptr->error != 0) {
		return -1;
	}
	return 0;
}