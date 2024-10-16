static int ldb_kv_write_index_dn_guid(struct ldb_module *module,
				      const struct ldb_message *msg,
				      int add)
{
	int ret;
	struct ldb_kv_private *ldb_kv = talloc_get_type(
	    ldb_module_get_private(module), struct ldb_kv_private);

	/* We index for DN only if using a GUID index */
	if (ldb_kv->cache->GUID_index_attribute == NULL) {
		return LDB_SUCCESS;
	}

	ret = ldb_kv_modify_index_dn(
	    module, ldb_kv, msg, msg->dn, LDB_KV_IDXDN, add);

	if (ret == LDB_ERR_CONSTRAINT_VIOLATION) {
		ldb_asprintf_errstring(ldb_module_get_ctx(module),
				       "Entry %s already exists",
				       ldb_dn_get_linearized(msg->dn));
		ret = LDB_ERR_ENTRY_ALREADY_EXISTS;
	}
	return ret;
}