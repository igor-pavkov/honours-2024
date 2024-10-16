static int ldb_kv_index_dn_and(struct ldb_module *module,
			       struct ldb_kv_private *ldb_kv,
			       const struct ldb_parse_tree *tree,
			       struct dn_list *list)
{
	struct ldb_context *ldb;
	unsigned int i;
	bool found;

	ldb = ldb_module_get_ctx(module);

	list->dn = NULL;
	list->count = 0;

	/* in the first pass we only look for unique simple
	   equality tests, in the hope of avoiding having to look
	   at any others */
	for (i=0; i<tree->u.list.num_elements; i++) {
		const struct ldb_parse_tree *subtree = tree->u.list.elements[i];
		int ret;

		if (subtree->operation != LDB_OP_EQUALITY ||
		    !ldb_kv_index_unique(
			ldb, ldb_kv, subtree->u.equality.attr)) {
			continue;
		}

		ret = ldb_kv_index_dn(module, ldb_kv, subtree, list);
		if (ret == LDB_ERR_NO_SUCH_OBJECT) {
			/* 0 && X == 0 */
			return LDB_ERR_NO_SUCH_OBJECT;
		}
		if (ret == LDB_SUCCESS) {
			/* a unique index match means we can
			 * stop. Note that we don't care if we return
			 * a few too many objects, due to later
			 * filtering */
			return LDB_SUCCESS;
		}
	}

	/* now do a full intersection */
	found = false;

	for (i=0; i<tree->u.list.num_elements; i++) {
		const struct ldb_parse_tree *subtree = tree->u.list.elements[i];
		struct dn_list *list2;
		int ret;

		list2 = talloc_zero(list, struct dn_list);
		if (list2 == NULL) {
			return ldb_module_oom(module);
		}

		ret = ldb_kv_index_dn(module, ldb_kv, subtree, list2);

		if (ret == LDB_ERR_NO_SUCH_OBJECT) {
			/* X && 0 == 0 */
			list->dn = NULL;
			list->count = 0;
			talloc_free(list2);
			return LDB_ERR_NO_SUCH_OBJECT;
		}

		if (ret != LDB_SUCCESS) {
			/* this didn't adding anything */
			talloc_free(list2);
			continue;
		}

		if (!found) {
			talloc_reparent(list2, list, list->dn);
			list->dn = list2->dn;
			list->count = list2->count;
			found = true;
		} else if (!list_intersect(ldb, ldb_kv, list, list2)) {
			talloc_free(list2);
			return LDB_ERR_OPERATIONS_ERROR;
		}

		if (list->count == 0) {
			list->dn = NULL;
			return LDB_ERR_NO_SUCH_OBJECT;
		}

		if (list->count < 2) {
			/* it isn't worth loading the next part of the tree */
			return LDB_SUCCESS;
		}
	}

	if (!found) {
		/* none of the attributes were indexed */
		return LDB_ERR_OPERATIONS_ERROR;
	}

	return LDB_SUCCESS;
}