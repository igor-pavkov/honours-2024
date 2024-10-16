static int ldb_kv_index_dn_or(struct ldb_module *module,
			      struct ldb_kv_private *ldb_kv,
			      const struct ldb_parse_tree *tree,
			      struct dn_list *list)
{
	struct ldb_context *ldb;
	unsigned int i;

	ldb = ldb_module_get_ctx(module);

	list->dn = NULL;
	list->count = 0;

	for (i=0; i<tree->u.list.num_elements; i++) {
		struct dn_list *list2;
		int ret;

		list2 = talloc_zero(list, struct dn_list);
		if (list2 == NULL) {
			return LDB_ERR_OPERATIONS_ERROR;
		}

		ret = ldb_kv_index_dn(
		    module, ldb_kv, tree->u.list.elements[i], list2);

		if (ret == LDB_ERR_NO_SUCH_OBJECT) {
			/* X || 0 == X */
			talloc_free(list2);
			continue;
		}

		if (ret != LDB_SUCCESS) {
			/* X || * == * */
			talloc_free(list2);
			return ret;
		}

		if (!list_union(ldb, ldb_kv, list, list2)) {
			talloc_free(list2);
			return LDB_ERR_OPERATIONS_ERROR;
		}
	}

	if (list->count == 0) {
		return LDB_ERR_NO_SUCH_OBJECT;
	}

	return LDB_SUCCESS;
}