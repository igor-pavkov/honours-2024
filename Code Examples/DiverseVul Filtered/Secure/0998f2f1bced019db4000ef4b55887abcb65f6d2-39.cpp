static int ldb_kv_index_add_all(struct ldb_module *module,
				struct ldb_kv_private *ldb_kv,
				const struct ldb_message *msg)
{
	struct ldb_message_element *elements = msg->elements;
	unsigned int i;
	const char *dn_str;
	int ret;

	if (ldb_dn_is_special(msg->dn)) {
		return LDB_SUCCESS;
	}

	dn_str = ldb_dn_get_linearized(msg->dn);
	if (dn_str == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	ret = ldb_kv_write_index_dn_guid(module, msg, 1);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	if (!ldb_kv->cache->attribute_indexes) {
		/* no indexed fields */
		return LDB_SUCCESS;
	}

	for (i = 0; i < msg->num_elements; i++) {
		if (!ldb_kv_is_indexed(module, ldb_kv, elements[i].name)) {
			continue;
		}
		ret = ldb_kv_index_add_el(module, ldb_kv, msg, &elements[i]);
		if (ret != LDB_SUCCESS) {
			struct ldb_context *ldb = ldb_module_get_ctx(module);
			ldb_asprintf_errstring(ldb,
					       __location__ ": Failed to re-index %s in %s - %s",
					       elements[i].name, dn_str,
					       ldb_errstring(ldb));
			return ret;
		}
	}

	return LDB_SUCCESS;
}