_PRIVATE_ int ldb_register_extended_match_rules(struct ldb_context *ldb)
{
	struct ldb_extended_match_rule *bitmask_and;
	struct ldb_extended_match_rule *bitmask_or;
	struct ldb_extended_match_rule *always_false;
	int ret;

	/* Register bitmask-and match */
	bitmask_and = talloc_zero(ldb, struct ldb_extended_match_rule);
	if (bitmask_and == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	bitmask_and->oid = LDB_OID_COMPARATOR_AND;
	bitmask_and->callback = ldb_match_bitmask;

	ret = ldb_register_extended_match_rule(ldb, bitmask_and);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	/* Register bitmask-or match */
	bitmask_or = talloc_zero(ldb, struct ldb_extended_match_rule);
	if (bitmask_or == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	bitmask_or->oid = LDB_OID_COMPARATOR_OR;
	bitmask_or->callback = ldb_match_bitmask;

	ret = ldb_register_extended_match_rule(ldb, bitmask_or);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	/* Register always-false match */
	always_false = talloc_zero(ldb, struct ldb_extended_match_rule);
	if (always_false == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	always_false->oid = SAMBA_LDAP_MATCH_ALWAYS_FALSE;
	always_false->callback = ldb_comparator_false;

	ret = ldb_register_extended_match_rule(ldb, always_false);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	return LDB_SUCCESS;
}