int ldb_match_msg_error(struct ldb_context *ldb,
			const struct ldb_message *msg,
			const struct ldb_parse_tree *tree,
			struct ldb_dn *base,
			enum ldb_scope scope,
			bool *matched)
{
	if ( ! ldb_match_scope(ldb, base, msg->dn, scope) ) {
		*matched = false;
		return LDB_SUCCESS;
	}

	return ldb_match_message(ldb, msg, tree, scope, matched);
}