int ldb_match_message(struct ldb_context *ldb,
		      const struct ldb_message *msg,
		      const struct ldb_parse_tree *tree,
		      enum ldb_scope scope, bool *matched)
{
	unsigned int i;
	int ret;

	*matched = false;

	if (scope != LDB_SCOPE_BASE && ldb_dn_is_special(msg->dn)) {
		/* don't match special records except on base searches */
		return LDB_SUCCESS;
	}

	switch (tree->operation) {
	case LDB_OP_AND:
		for (i=0;i<tree->u.list.num_elements;i++) {
			ret = ldb_match_message(ldb, msg, tree->u.list.elements[i], scope, matched);
			if (ret != LDB_SUCCESS) return ret;
			if (!*matched) return LDB_SUCCESS;
		}
		*matched = true;
		return LDB_SUCCESS;

	case LDB_OP_OR:
		for (i=0;i<tree->u.list.num_elements;i++) {
			ret = ldb_match_message(ldb, msg, tree->u.list.elements[i], scope, matched);
			if (ret != LDB_SUCCESS) return ret;
			if (*matched) return LDB_SUCCESS;
		}
		*matched = false;
		return LDB_SUCCESS;

	case LDB_OP_NOT:
		ret = ldb_match_message(ldb, msg, tree->u.isnot.child, scope, matched);
		if (ret != LDB_SUCCESS) return ret;
		*matched = ! *matched;
		return LDB_SUCCESS;

	case LDB_OP_EQUALITY:
		return ldb_match_equality(ldb, msg, tree, scope, matched);

	case LDB_OP_SUBSTRING:
		return ldb_match_substring(ldb, msg, tree, scope, matched);

	case LDB_OP_GREATER:
		return ldb_match_comparison(ldb, msg, tree, scope, LDB_OP_GREATER, matched);

	case LDB_OP_LESS:
		return ldb_match_comparison(ldb, msg, tree, scope, LDB_OP_LESS, matched);

	case LDB_OP_PRESENT:
		return ldb_match_present(ldb, msg, tree, scope, matched);

	case LDB_OP_APPROX:
		return ldb_match_comparison(ldb, msg, tree, scope, LDB_OP_APPROX, matched);

	case LDB_OP_EXTENDED:
		return ldb_match_extended(ldb, msg, tree, scope, matched);
	}

	return LDB_ERR_INAPPROPRIATE_MATCHING;
}