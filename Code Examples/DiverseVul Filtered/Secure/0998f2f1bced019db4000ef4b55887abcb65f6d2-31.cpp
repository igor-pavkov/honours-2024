static int ldb_kv_index_dn_simple(struct ldb_module *module,
				  struct ldb_kv_private *ldb_kv,
				  const struct ldb_parse_tree *tree,
				  struct dn_list *list)
{
	struct ldb_context *ldb;
	struct ldb_dn *dn;
	int ret;
	enum key_truncation truncation = KEY_NOT_TRUNCATED;

	ldb = ldb_module_get_ctx(module);

	list->count = 0;
	list->dn = NULL;

	/* if the attribute isn't in the list of indexed attributes then
	   this node needs a full search */
	if (!ldb_kv_is_indexed(module, ldb_kv, tree->u.equality.attr)) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	/* the attribute is indexed. Pull the list of DNs that match the
	   search criterion */
	dn = ldb_kv_index_key(ldb,
			      ldb_kv,
			      tree->u.equality.attr,
			      &tree->u.equality.value,
			      NULL,
			      &truncation);
	/*
	 * We ignore truncation here and allow multi-valued matches
	 * as ltdb_search_indexed will filter out the wrong one in
	 * ltdb_index_filter() which calls ldb_match_message().
	 */
	if (!dn) return LDB_ERR_OPERATIONS_ERROR;

	ret = ldb_kv_dn_list_load(module, ldb_kv, dn, list);
	talloc_free(dn);
	return ret;
}