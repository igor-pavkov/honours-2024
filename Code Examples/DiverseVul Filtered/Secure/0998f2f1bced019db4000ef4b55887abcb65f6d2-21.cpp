int ldb_kv_key_dn_from_idx(struct ldb_module *module,
			   struct ldb_kv_private *ldb_kv,
			   TALLOC_CTX *mem_ctx,
			   struct ldb_dn *dn,
			   struct ldb_val *ldb_key)
{
	struct ldb_context *ldb = ldb_module_get_ctx(module);
	int ret;
	int index = 0;
	enum key_truncation truncation = KEY_NOT_TRUNCATED;
	struct dn_list *list = talloc(mem_ctx, struct dn_list);
	if (list == NULL) {
		ldb_oom(ldb);
		return LDB_ERR_OPERATIONS_ERROR;
	}

	ret = ldb_kv_index_dn_base_dn(module, ldb_kv, dn, list, &truncation);
	if (ret != LDB_SUCCESS) {
		TALLOC_FREE(list);
		return ret;
	}

	if (list->count == 0) {
		TALLOC_FREE(list);
		return LDB_ERR_NO_SUCH_OBJECT;
	}

	if (list->count > 1 && truncation == KEY_NOT_TRUNCATED)  {
		const char *dn_str = ldb_dn_get_linearized(dn);
		ldb_asprintf_errstring(ldb_module_get_ctx(module),
				       __location__
				       ": Failed to read DN index "
				       "against %s for %s: too many "
				       "values (%u > 1)",
				       ldb_kv->cache->GUID_index_attribute,
				       dn_str,
				       list->count);
		TALLOC_FREE(list);
		return LDB_ERR_CONSTRAINT_VIOLATION;
	}

	if (list->count > 0 && truncation == KEY_TRUNCATED)  {
		/*
		 * DN key has been truncated, need to inspect the actual
		 * records to locate the actual DN
		 */
		int i;
		index = -1;
		for (i=0; i < list->count; i++) {
			uint8_t guid_key[LDB_KV_GUID_KEY_SIZE];
			struct ldb_val key = {
				.data = guid_key,
				.length = sizeof(guid_key)
			};
			const int flags = LDB_UNPACK_DATA_FLAG_NO_ATTRS;
			struct ldb_message *rec = ldb_msg_new(ldb);
			if (rec == NULL) {
				TALLOC_FREE(list);
				return LDB_ERR_OPERATIONS_ERROR;
			}

			ret = ldb_kv_idx_to_key(
			    module, ldb_kv, ldb, &list->dn[i], &key);
			if (ret != LDB_SUCCESS) {
				TALLOC_FREE(list);
				TALLOC_FREE(rec);
				return ret;
			}

			ret =
			    ldb_kv_search_key(module, ldb_kv, key, rec, flags);
			if (key.data != guid_key) {
				TALLOC_FREE(key.data);
			}
			if (ret == LDB_ERR_NO_SUCH_OBJECT) {
				/*
				 * the record has disappeared?
				 * yes, this can happen
				 */
				TALLOC_FREE(rec);
				continue;
			}

			if (ret != LDB_SUCCESS) {
				/* an internal error */
				TALLOC_FREE(rec);
				TALLOC_FREE(list);
				return LDB_ERR_OPERATIONS_ERROR;
			}

			/*
			 * We found the actual DN that we wanted from in the
			 * multiple values that matched the index
			 * (due to truncation), so return that.
			 *
			 */
			if (ldb_dn_compare(dn, rec->dn) == 0) {
				index = i;
				TALLOC_FREE(rec);
				break;
			}
		}

		/*
		 * We matched the index but the actual DN we wanted
		 * was not here.
		 */
		if (index == -1) {
			TALLOC_FREE(list);
			return LDB_ERR_NO_SUCH_OBJECT;
		}
	}

	/* The ldb_key memory is allocated by the caller */
	ret = ldb_kv_guid_to_key(module, ldb_kv, &list->dn[index], ldb_key);
	TALLOC_FREE(list);

	if (ret != LDB_SUCCESS) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	return LDB_SUCCESS;
}