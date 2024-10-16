static int ldb_kv_index_onelevel(struct ldb_module *module,
				 const struct ldb_message *msg,
				 int add)
{
	struct ldb_kv_private *ldb_kv = talloc_get_type(
	    ldb_module_get_private(module), struct ldb_kv_private);
	struct ldb_dn *pdn;
	int ret;

	/* We index for ONE Level only if requested */
	if (!ldb_kv->cache->one_level_indexes) {
		return LDB_SUCCESS;
	}

	pdn = ldb_dn_get_parent(module, msg->dn);
	if (pdn == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}
	ret =
	    ldb_kv_modify_index_dn(module, ldb_kv, msg, pdn, LDB_KV_IDXONE, add);

	talloc_free(pdn);

	return ret;
}