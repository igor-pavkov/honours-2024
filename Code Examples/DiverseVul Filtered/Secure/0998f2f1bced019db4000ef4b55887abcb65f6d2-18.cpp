int ldb_kv_index_del_element(struct ldb_module *module,
			     struct ldb_kv_private *ldb_kv,
			     const struct ldb_message *msg,
			     struct ldb_message_element *el)
{
	const char *dn_str;
	int ret;
	unsigned int i;

	if (!ldb_kv->cache->attribute_indexes) {
		/* no indexed fields */
		return LDB_SUCCESS;
	}

	dn_str = ldb_dn_get_linearized(msg->dn);
	if (dn_str == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	if (dn_str[0] == '@') {
		return LDB_SUCCESS;
	}

	if (!ldb_kv_is_indexed(module, ldb_kv, el->name)) {
		return LDB_SUCCESS;
	}
	for (i = 0; i < el->num_values; i++) {
		ret = ldb_kv_index_del_value(module, ldb_kv, msg, el, i);
		if (ret != LDB_SUCCESS) {
			return ret;
		}
	}

	return LDB_SUCCESS;
}