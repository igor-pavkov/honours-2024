static int ldb_kv_index_dn_not(struct ldb_module *module,
			       struct ldb_kv_private *ldb_kv,
			       const struct ldb_parse_tree *tree,
			       struct dn_list *list)
{
	/* the only way to do an indexed not would be if we could
	   negate the not via another not or if we knew the total
	   number of database elements so we could know that the
	   existing expression covered the whole database.

	   instead, we just give up, and rely on a full index scan
	   (unless an outer & manages to reduce the list)
	*/
	return LDB_ERR_OPERATIONS_ERROR;
}