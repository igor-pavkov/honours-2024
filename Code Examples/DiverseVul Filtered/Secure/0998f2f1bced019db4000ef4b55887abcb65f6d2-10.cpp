int ldb_kv_index_delete(struct ldb_module *module,
			const struct ldb_message *msg)
{
	struct ldb_kv_private *ldb_kv = talloc_get_type(
	    ldb_module_get_private(module), struct ldb_kv_private);
	int ret;
	unsigned int i;

	if (ldb_dn_is_special(msg->dn)) {
		return LDB_SUCCESS;
	}

	ret = ldb_kv_index_onelevel(module, msg, 0);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	ret = ldb_kv_write_index_dn_guid(module, msg, 0);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	if (!ldb_kv->cache->attribute_indexes) {
		/* no indexed fields */
		return LDB_SUCCESS;
	}

	for (i = 0; i < msg->num_elements; i++) {
		ret = ldb_kv_index_del_element(
		    module, ldb_kv, msg, &msg->elements[i]);
		if (ret != LDB_SUCCESS) {
			return ret;
		}
	}

	return LDB_SUCCESS;
}