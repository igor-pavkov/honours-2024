static bool ldb_kv_is_indexed(struct ldb_module *module,
			      struct ldb_kv_private *ldb_kv,
			      const char *attr)
{
	struct ldb_context *ldb = ldb_module_get_ctx(module);
	unsigned int i;
	struct ldb_message_element *el;

	if ((ldb_kv->cache->GUID_index_attribute != NULL) &&
	    (ldb_attr_cmp(attr, ldb_kv->cache->GUID_index_attribute) == 0)) {
		/* Implicity covered, this is the index key */
		return false;
	}
	if (ldb->schema.index_handler_override) {
		const struct ldb_schema_attribute *a
			= ldb_schema_attribute_by_name(ldb, attr);

		if (a == NULL) {
			return false;
		}

		if (a->flags & LDB_ATTR_FLAG_INDEXED) {
			return true;
		} else {
			return false;
		}
	}

	if (!ldb_kv->cache->attribute_indexes) {
		return false;
	}

	el = ldb_msg_find_element(ldb_kv->cache->indexlist, LDB_KV_IDXATTR);
	if (el == NULL) {
		return false;
	}

	/* TODO: this is too expensive! At least use a binary search */
	for (i=0; i<el->num_values; i++) {
		if (ldb_attr_cmp((char *)el->values[i].data, attr) == 0) {
			return true;
		}
	}
	return false;
}