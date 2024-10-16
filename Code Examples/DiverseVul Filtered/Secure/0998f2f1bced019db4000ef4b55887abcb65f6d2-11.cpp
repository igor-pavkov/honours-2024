static int delete_index(struct ldb_kv_private *ldb_kv,
			struct ldb_val key,
			struct ldb_val data,
			void *state)
{
	struct ldb_module *module = state;
	const char *dnstr = "DN=" LDB_KV_INDEX ":";
	struct dn_list list;
	struct ldb_dn *dn;
	struct ldb_val v;
	int ret;

	if (strncmp((char *)key.data, dnstr, strlen(dnstr)) != 0) {
		return 0;
	}
	/* we need to put a empty list in the internal tdb for this
	 * index entry */
	list.dn = NULL;
	list.count = 0;

	/* the offset of 3 is to remove the DN= prefix. */
	v.data = key.data + 3;
	v.length = strnlen((char *)key.data, key.length) - 3;

	dn = ldb_dn_from_ldb_val(ldb_kv, ldb_module_get_ctx(module), &v);

	/*
	 * This does not actually touch the DB quite yet, just
         * the in-memory index cache
	 */
	ret = ldb_kv_dn_list_store(module, dn, &list);
	if (ret != LDB_SUCCESS) {
		ldb_asprintf_errstring(ldb_module_get_ctx(module),
				       "Unable to store null index for %s\n",
						ldb_dn_get_linearized(dn));
		talloc_free(dn);
		return -1;
	}
	talloc_free(dn);
	return 0;
}