int ldb_kv_index_add_element(struct ldb_module *module,
			     struct ldb_kv_private *ldb_kv,
			     const struct ldb_message *msg,
			     struct ldb_message_element *el)
{
	if (ldb_dn_is_special(msg->dn)) {
		return LDB_SUCCESS;
	}
	if (!ldb_kv_is_indexed(module, ldb_kv, el->name)) {
		return LDB_SUCCESS;
	}
	return ldb_kv_index_add_el(module, ldb_kv, msg, el);
}