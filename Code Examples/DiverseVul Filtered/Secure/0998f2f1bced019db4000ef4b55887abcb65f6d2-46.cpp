int ldb_kv_index_add_new(struct ldb_module *module,
			 struct ldb_kv_private *ldb_kv,
			 const struct ldb_message *msg)
{
	int ret;

	if (ldb_dn_is_special(msg->dn)) {
		return LDB_SUCCESS;
	}

	ret = ldb_kv_index_add_all(module, ldb_kv, msg);
	if (ret != LDB_SUCCESS) {
		/*
		 * Because we can't trust the caller to be doing
		 * transactions properly, clean up any index for this
		 * entry rather than relying on a transaction
		 * cleanup
		 */

		ldb_kv_index_delete(module, msg);
		return ret;
	}

	ret = ldb_kv_index_onelevel(module, msg, 1);
	if (ret != LDB_SUCCESS) {
		/*
		 * Because we can't trust the caller to be doing
		 * transactions properly, clean up any index for this
		 * entry rather than relying on a transaction
		 * cleanup
		 */
		ldb_kv_index_delete(module, msg);
		return ret;
	}
	return ret;
}