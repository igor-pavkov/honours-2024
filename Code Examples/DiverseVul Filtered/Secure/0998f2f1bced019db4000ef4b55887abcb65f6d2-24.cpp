static int ldb_kv_index_dn(struct ldb_module *module,
			   struct ldb_kv_private *ldb_kv,
			   const struct ldb_parse_tree *tree,
			   struct dn_list *list)
{
	int ret = LDB_ERR_OPERATIONS_ERROR;

	switch (tree->operation) {
	case LDB_OP_AND:
		ret = ldb_kv_index_dn_and(module, ldb_kv, tree, list);
		break;

	case LDB_OP_OR:
		ret = ldb_kv_index_dn_or(module, ldb_kv, tree, list);
		break;

	case LDB_OP_NOT:
		ret = ldb_kv_index_dn_not(module, ldb_kv, tree, list);
		break;

	case LDB_OP_EQUALITY:
		ret = ldb_kv_index_dn_leaf(module, ldb_kv, tree, list);
		break;

	case LDB_OP_SUBSTRING:
	case LDB_OP_GREATER:
	case LDB_OP_LESS:
	case LDB_OP_PRESENT:
	case LDB_OP_APPROX:
	case LDB_OP_EXTENDED:
		/* we can't index with fancy bitops yet */
		ret = LDB_ERR_OPERATIONS_ERROR;
		break;
	}

	return ret;
}