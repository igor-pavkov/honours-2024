static int ldb_match_substring(struct ldb_context *ldb, 
			       const struct ldb_message *msg,
			       const struct ldb_parse_tree *tree,
			       enum ldb_scope scope, bool *matched)
{
	unsigned int i;
	struct ldb_message_element *el;

	el = ldb_msg_find_element(msg, tree->u.substring.attr);
	if (el == NULL) {
		*matched = false;
		return LDB_SUCCESS;
	}

	for (i = 0; i < el->num_values; i++) {
		int ret;
		ret = ldb_wildcard_compare(ldb, tree, el->values[i], matched);
		if (ret != LDB_SUCCESS) return ret;
		if (*matched) return LDB_SUCCESS;
	}

	*matched = false;
	return LDB_SUCCESS;
}