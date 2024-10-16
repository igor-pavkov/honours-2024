static int ldb_match_extended(struct ldb_context *ldb, 
			      const struct ldb_message *msg,
			      const struct ldb_parse_tree *tree,
			      enum ldb_scope scope, bool *matched)
{
	const struct ldb_extended_match_rule *rule;

	if (tree->u.extended.dnAttributes) {
		/* FIXME: We really need to find out what this ":dn" part in
		 * an extended match means and how to handle it. For now print
		 * only a warning to have s3 winbind and other tools working
		 * against us. - Matthias */
		ldb_debug(ldb, LDB_DEBUG_WARNING, "ldb: dnAttributes extended match not supported yet");
	}
	if (tree->u.extended.rule_id == NULL) {
		ldb_debug(ldb, LDB_DEBUG_ERROR, "ldb: no-rule extended matches not supported yet");
		return LDB_ERR_INAPPROPRIATE_MATCHING;
	}
	if (tree->u.extended.attr == NULL) {
		ldb_debug(ldb, LDB_DEBUG_ERROR, "ldb: no-attribute extended matches not supported yet");
		return LDB_ERR_INAPPROPRIATE_MATCHING;
	}

	rule = ldb_find_extended_match_rule(ldb, tree->u.extended.rule_id);
	if (rule == NULL) {
		*matched = false;
		ldb_debug(ldb, LDB_DEBUG_ERROR, "ldb: unknown extended rule_id %s",
			  tree->u.extended.rule_id);
		return LDB_SUCCESS;
	}

	return rule->callback(ldb, rule->oid, msg,
			      tree->u.extended.attr,
			      &tree->u.extended.value, matched);
}