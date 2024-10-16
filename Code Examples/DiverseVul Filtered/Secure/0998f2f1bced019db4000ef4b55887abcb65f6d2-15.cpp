int ldb_kv_search_indexed(struct ldb_kv_context *ac, uint32_t *match_count)
{
	struct ldb_context *ldb = ldb_module_get_ctx(ac->module);
	struct ldb_kv_private *ldb_kv = talloc_get_type(
	    ldb_module_get_private(ac->module), struct ldb_kv_private);
	struct dn_list *dn_list;
	int ret;
	enum ldb_scope index_scope;
	enum key_truncation scope_one_truncation = KEY_NOT_TRUNCATED;

	/* see if indexing is enabled */
	if (!ldb_kv->cache->attribute_indexes &&
	    !ldb_kv->cache->one_level_indexes && ac->scope != LDB_SCOPE_BASE) {
		/* fallback to a full search */
		return LDB_ERR_OPERATIONS_ERROR;
	}

	dn_list = talloc_zero(ac, struct dn_list);
	if (dn_list == NULL) {
		return ldb_module_oom(ac->module);
	}

	/*
	 * For the purposes of selecting the switch arm below, if we
	 * don't have a one-level index then treat it like a subtree
	 * search
	 */
	if (ac->scope == LDB_SCOPE_ONELEVEL &&
	    !ldb_kv->cache->one_level_indexes) {
		index_scope = LDB_SCOPE_SUBTREE;
	} else {
		index_scope = ac->scope;
	}

	switch (index_scope) {
	case LDB_SCOPE_BASE:
		/*
		 * The only caller will have filtered the operation out
		 * so we should never get here
		 */
		return ldb_operr(ldb);

	case LDB_SCOPE_ONELEVEL:
		/*
		 * If we ever start to also load the index values for
		 * the tree, we must ensure we strictly intersect with
		 * this list, as we trust the ONELEVEL index
		 */
		ret = ldb_kv_index_dn_one(ac->module,
					  ldb_kv,
					  ac->base,
					  dn_list,
					  &scope_one_truncation);
		if (ret != LDB_SUCCESS) {
			talloc_free(dn_list);
			return ret;
		}

		/*
		 * If we have too many matches, running the filter
		 * tree over the SCOPE_ONELEVEL can be quite expensive
		 * so we now check the filter tree index as well.
		 *
		 * We only do this in the GUID index mode, which is
		 * O(n*log(m)) otherwise the intersection below will
		 * be too costly at O(n*m).
		 *
		 * We don't set a heuristic for 'too many' but instead
		 * do it always and rely on the index lookup being
		 * fast enough in the small case.
		 */
		if (ldb_kv->cache->GUID_index_attribute != NULL) {
			struct dn_list *idx_one_tree_list
				= talloc_zero(ac, struct dn_list);
			if (idx_one_tree_list == NULL) {
				return ldb_module_oom(ac->module);
			}

			if (!ldb_kv->cache->attribute_indexes) {
				talloc_free(idx_one_tree_list);
				talloc_free(dn_list);
				return LDB_ERR_OPERATIONS_ERROR;
			}
			/*
			 * Here we load the index for the tree.
			 *
			 * We only care if this is successful, if the
			 * index can't trim the result list down then
			 * the ONELEVEL index is still good enough.
			 */
			ret = ldb_kv_index_dn(
			    ac->module, ldb_kv, ac->tree, idx_one_tree_list);
			if (ret == LDB_SUCCESS) {
				if (!list_intersect(ldb,
						    ldb_kv,
						    dn_list,
						    idx_one_tree_list)) {
					talloc_free(idx_one_tree_list);
					talloc_free(dn_list);
					return LDB_ERR_OPERATIONS_ERROR;
				}
			}
		}
		break;

	case LDB_SCOPE_SUBTREE:
	case LDB_SCOPE_DEFAULT:
		if (!ldb_kv->cache->attribute_indexes) {
			talloc_free(dn_list);
			return LDB_ERR_OPERATIONS_ERROR;
		}
		/*
		 * Here we load the index for the tree.  We have no
		 * index for the subtree.
		 */
		ret = ldb_kv_index_dn(ac->module, ldb_kv, ac->tree, dn_list);
		if (ret != LDB_SUCCESS) {
			talloc_free(dn_list);
			return ret;
		}
		break;
	}

	/*
	 * It is critical that this function do the re-filter even
	 * on things found by the index as the index can over-match
	 * in cases of truncation (as well as when it decides it is
	 * not worth further filtering)
	 *
	 * If this changes, then the index code above would need to
	 * pass up a flag to say if any index was truncated during
	 * processing as the truncation here refers only to the
	 * SCOPE_ONELEVEL index.
	 */
	ret = ldb_kv_index_filter(
	    ldb_kv, dn_list, ac, match_count, scope_one_truncation);
	talloc_free(dn_list);
	return ret;
}