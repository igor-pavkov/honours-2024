static int ldb_match_scope(struct ldb_context *ldb,
			   struct ldb_dn *base,
			   struct ldb_dn *dn,
			   enum ldb_scope scope)
{
	int ret = 0;

	if (base == NULL || dn == NULL) {
		return 1;
	}

	switch (scope) {
	case LDB_SCOPE_BASE:
		if (ldb_dn_compare(base, dn) == 0) {
			ret = 1;
		}
		break;

	case LDB_SCOPE_ONELEVEL:
		if (ldb_dn_get_comp_num(dn) == (ldb_dn_get_comp_num(base) + 1)) {
			if (ldb_dn_compare_base(base, dn) == 0) {
				ret = 1;
			}
		}
		break;
		
	case LDB_SCOPE_SUBTREE:
	default:
		if (ldb_dn_compare_base(base, dn) == 0) {
			ret = 1;
		}
		break;
	}

	return ret;
}