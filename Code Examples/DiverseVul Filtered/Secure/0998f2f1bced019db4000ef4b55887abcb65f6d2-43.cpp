static int ldb_kv_index_add_el(struct ldb_module *module,
			       struct ldb_kv_private *ldb_kv,
			       const struct ldb_message *msg,
			       struct ldb_message_element *el)
{
	unsigned int i;
	for (i = 0; i < el->num_values; i++) {
		int ret = ldb_kv_index_add1(module, ldb_kv, msg, el, i);
		if (ret != LDB_SUCCESS) {
			return ret;
		}
	}

	return LDB_SUCCESS;
}