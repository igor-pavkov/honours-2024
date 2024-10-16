int ldb_register_extended_match_rule(struct ldb_context *ldb,
				     const struct ldb_extended_match_rule *rule)
{
	const struct ldb_extended_match_rule *lookup_rule;
	struct ldb_extended_match_entry *entry;

	lookup_rule = ldb_find_extended_match_rule(ldb, rule->oid);
	if (lookup_rule) {
		return LDB_ERR_ENTRY_ALREADY_EXISTS;
	}

	entry = talloc_zero(ldb, struct ldb_extended_match_entry);
	if (!entry) {
		return LDB_ERR_OPERATIONS_ERROR;
	}
	entry->rule = rule;
	DLIST_ADD_END(ldb->extended_match_rules, entry);

	return LDB_SUCCESS;
}