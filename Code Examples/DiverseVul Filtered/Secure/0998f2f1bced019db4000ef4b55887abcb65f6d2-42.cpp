static int ldb_kv_index_dn_leaf(struct ldb_module *module,
				struct ldb_kv_private *ldb_kv,
				const struct ldb_parse_tree *tree,
				struct dn_list *list)
{
	if (ldb_kv->disallow_dn_filter &&
	    (ldb_attr_cmp(tree->u.equality.attr, "dn") == 0)) {
		/* in AD mode we do not support "(dn=...)" search filters */
		list->dn = NULL;
		list->count = 0;
		return LDB_SUCCESS;
	}
	if (tree->u.equality.attr[0] == '@') {
		/* Do not allow a indexed search against an @ */
		list->dn = NULL;
		list->count = 0;
		return LDB_SUCCESS;
	}
	if (ldb_attr_dn(tree->u.equality.attr) == 0) {
		enum key_truncation truncation = KEY_NOT_TRUNCATED;
		struct ldb_dn *dn
			= ldb_dn_from_ldb_val(list,
					      ldb_module_get_ctx(module),
					      &tree->u.equality.value);
		if (dn == NULL) {
			/* If we can't parse it, no match */
			list->dn = NULL;
			list->count = 0;
			return LDB_SUCCESS;
		}

		/*
		 * Re-use the same code we use for a SCOPE_BASE
		 * search
		 *
		 * We can't call TALLOC_FREE(dn) as this must belong
		 * to list for the memory to remain valid.
		 */
		return ldb_kv_index_dn_base_dn(
		    module, ldb_kv, dn, list, &truncation);
		/*
		 * We ignore truncation here and allow multi-valued matches
		 * as ltdb_search_indexed will filter out the wrong one in
		 * ltdb_index_filter() which calls ldb_match_message().
		 */

	} else if ((ldb_kv->cache->GUID_index_attribute != NULL) &&
		   (ldb_attr_cmp(tree->u.equality.attr,
				 ldb_kv->cache->GUID_index_attribute) == 0)) {
		int ret;
		struct ldb_context *ldb = ldb_module_get_ctx(module);
		list->dn = talloc_array(list, struct ldb_val, 1);
		if (list->dn == NULL) {
			ldb_module_oom(module);
			return LDB_ERR_OPERATIONS_ERROR;
		}
		/*
		 * We need to go via the canonicalise_fn() to
		 * ensure we get the index in binary, rather
		 * than a string
		 */
		ret = ldb_kv->GUID_index_syntax->canonicalise_fn(
		    ldb, list->dn, &tree->u.equality.value, &list->dn[0]);
		if (ret != LDB_SUCCESS) {
			return LDB_ERR_OPERATIONS_ERROR;
		}
		list->count = 1;
		return LDB_SUCCESS;
	}

	return ldb_kv_index_dn_simple(module, ldb_kv, tree, list);
}