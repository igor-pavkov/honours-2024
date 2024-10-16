int ldb_match_msg(struct ldb_context *ldb,
		  const struct ldb_message *msg,
		  const struct ldb_parse_tree *tree,
		  struct ldb_dn *base,
		  enum ldb_scope scope)
{
	bool matched;
	int ret;

	if ( ! ldb_match_scope(ldb, base, msg->dn, scope) ) {
		return 0;
	}

	ret = ldb_match_message(ldb, msg, tree, scope, &matched);
	if (ret != LDB_SUCCESS) {
		/* to match the old API, we need to consider this a
		   failure to match */
		return 0;
	}
	return matched?1:0;
}