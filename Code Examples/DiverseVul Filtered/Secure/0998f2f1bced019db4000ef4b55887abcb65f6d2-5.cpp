static int ldb_kv_dn_list_store_full(struct ldb_module *module,
				     struct ldb_kv_private *ldb_kv,
				     struct ldb_dn *dn,
				     struct dn_list *list)
{
	struct ldb_message *msg;
	int ret;

	msg = ldb_msg_new(module);
	if (!msg) {
		return ldb_module_oom(module);
	}

	msg->dn = dn;

	if (list->count == 0) {
		ret = ldb_kv_delete_noindex(module, msg);
		if (ret == LDB_ERR_NO_SUCH_OBJECT) {
			ret = LDB_SUCCESS;
		}
		talloc_free(msg);
		return ret;
	}

	if (ldb_kv->cache->GUID_index_attribute == NULL) {
		ret = ldb_msg_add_fmt(msg, LDB_KV_IDXVERSION, "%u",
				      LDB_KV_INDEXING_VERSION);
		if (ret != LDB_SUCCESS) {
			talloc_free(msg);
			return ldb_module_oom(module);
		}
	} else {
		ret = ldb_msg_add_fmt(msg, LDB_KV_IDXVERSION, "%u",
				      LDB_KV_GUID_INDEXING_VERSION);
		if (ret != LDB_SUCCESS) {
			talloc_free(msg);
			return ldb_module_oom(module);
		}
	}

	if (list->count > 0) {
		struct ldb_message_element *el;

		ret = ldb_msg_add_empty(msg, LDB_KV_IDX, LDB_FLAG_MOD_ADD, &el);
		if (ret != LDB_SUCCESS) {
			talloc_free(msg);
			return ldb_module_oom(module);
		}

		if (ldb_kv->cache->GUID_index_attribute == NULL) {
			el->values = list->dn;
			el->num_values = list->count;
		} else {
			struct ldb_val v;
			unsigned int i;
			el->values = talloc_array(msg,
						  struct ldb_val, 1);
			if (el->values == NULL) {
				talloc_free(msg);
				return ldb_module_oom(module);
			}

			v.data = talloc_array_size(el->values,
						   list->count,
						   LDB_KV_GUID_SIZE);
			if (v.data == NULL) {
				talloc_free(msg);
				return ldb_module_oom(module);
			}

			v.length = talloc_get_size(v.data);

			for (i = 0; i < list->count; i++) {
				if (list->dn[i].length !=
				    LDB_KV_GUID_SIZE) {
					talloc_free(msg);
					return ldb_module_operr(module);
				}
				memcpy(&v.data[LDB_KV_GUID_SIZE*i],
				       list->dn[i].data,
				       LDB_KV_GUID_SIZE);
			}
			el->values[0] = v;
			el->num_values = 1;
		}
	}

	ret = ldb_kv_store(module, msg, TDB_REPLACE);
	talloc_free(msg);
	return ret;
}