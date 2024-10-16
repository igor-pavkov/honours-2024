static int ldb_match_present(struct ldb_context *ldb, 
			     const struct ldb_message *msg,
			     const struct ldb_parse_tree *tree,
			     enum ldb_scope scope, bool *matched)
{
	const struct ldb_schema_attribute *a;
	struct ldb_message_element *el;

	if (ldb_attr_dn(tree->u.present.attr) == 0) {
		*matched = true;
		return LDB_SUCCESS;
	}

	el = ldb_msg_find_element(msg, tree->u.present.attr);
	if (el == NULL) {
		*matched = false;
		return LDB_SUCCESS;
	}

	a = ldb_schema_attribute_by_name(ldb, el->name);
	if (!a) {
		return LDB_ERR_INVALID_ATTRIBUTE_SYNTAX;
	}

	if (a->syntax->operator_fn) {
		unsigned int i;
		for (i = 0; i < el->num_values; i++) {
			int ret = a->syntax->operator_fn(ldb, LDB_OP_PRESENT, a, &el->values[i], NULL, matched);
			if (ret != LDB_SUCCESS) return ret;
			if (*matched) return LDB_SUCCESS;
		}
		*matched = false;
		return LDB_SUCCESS;
	}

	*matched = true;
	return LDB_SUCCESS;
}