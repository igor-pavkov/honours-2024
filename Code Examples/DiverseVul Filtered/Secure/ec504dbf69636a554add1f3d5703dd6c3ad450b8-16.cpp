static const struct ldb_extended_match_rule *ldb_find_extended_match_rule(struct ldb_context *ldb,
									  const char *oid)
{
	struct ldb_extended_match_entry *extended_match_rule;

	for (extended_match_rule = ldb->extended_match_rules;
	     extended_match_rule;
	     extended_match_rule = extended_match_rule->next) {
		if (strcmp(extended_match_rule->rule->oid, oid) == 0) {
			return extended_match_rule->rule;
		}
	}

	return NULL;
}