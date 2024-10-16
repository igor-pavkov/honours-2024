static int ldb_match_comparison(struct ldb_context *ldb, 
				const struct ldb_message *msg,
				const struct ldb_parse_tree *tree,
				enum ldb_scope scope,
				enum ldb_parse_op comp_op, bool *matched)
{
	unsigned int i;
	struct ldb_message_element *el;
	const struct ldb_schema_attribute *a;

	/* FIXME: APPROX comparison not handled yet */
	if (comp_op == LDB_OP_APPROX) {
		return LDB_ERR_INAPPROPRIATE_MATCHING;
	}

	el = ldb_msg_find_element(msg, tree->u.comparison.attr);
	if (el == NULL) {
		*matched = false;
		return LDB_SUCCESS;
	}

	a = ldb_schema_attribute_by_name(ldb, el->name);
	if (!a) {
		return LDB_ERR_INVALID_ATTRIBUTE_SYNTAX;
	}

	for (i = 0; i < el->num_values; i++) {
		if (a->syntax->operator_fn) {
			int ret;
			ret = a->syntax->operator_fn(ldb, comp_op, a, &el->values[i], &tree->u.comparison.value, matched);
			if (ret != LDB_SUCCESS) return ret;
			if (*matched) return LDB_SUCCESS;
		} else {
			int ret = a->syntax->comparison_fn(ldb, ldb, &el->values[i], &tree->u.comparison.value);

			if (ret == 0) {
				*matched = true;
				return LDB_SUCCESS;
			}
			if (ret > 0 && comp_op == LDB_OP_GREATER) {
				*matched = true;
				return LDB_SUCCESS;
			}
			if (ret < 0 && comp_op == LDB_OP_LESS) {
				*matched = true;
				return LDB_SUCCESS;
			}
		}
	}

	*matched = false;
	return LDB_SUCCESS;
}