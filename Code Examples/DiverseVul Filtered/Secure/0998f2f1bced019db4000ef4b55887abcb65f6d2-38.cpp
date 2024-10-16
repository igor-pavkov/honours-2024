static int ldb_kv_index_add1(struct ldb_module *module,
			     struct ldb_kv_private *ldb_kv,
			     const struct ldb_message *msg,
			     struct ldb_message_element *el,
			     int v_idx)
{
	struct ldb_context *ldb;
	struct ldb_dn *dn_key;
	int ret;
	const struct ldb_schema_attribute *a;
	struct dn_list *list;
	unsigned alloc_len;
	enum key_truncation truncation = KEY_TRUNCATED;


	ldb = ldb_module_get_ctx(module);

	list = talloc_zero(module, struct dn_list);
	if (list == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	dn_key = ldb_kv_index_key(
	    ldb, ldb_kv, el->name, &el->values[v_idx], &a, &truncation);
	if (!dn_key) {
		talloc_free(list);
		return LDB_ERR_OPERATIONS_ERROR;
	}
	/*
	 * Samba only maintains unique indexes on the objectSID and objectGUID
	 * so if a unique index key exceeds the maximum length there is a
	 * problem.
	 */
	if ((truncation == KEY_TRUNCATED) && (a != NULL &&
		(a->flags & LDB_ATTR_FLAG_UNIQUE_INDEX ||
		(el->flags & LDB_FLAG_INTERNAL_FORCE_UNIQUE_INDEX)))) {

		ldb_asprintf_errstring(
		    ldb,
		    __location__ ": unique index key on %s in %s, "
				 "exceeds maximum key length of %u (encoded).",
		    el->name,
		    ldb_dn_get_linearized(msg->dn),
		    ldb_kv->max_key_length);
		talloc_free(list);
		return LDB_ERR_CONSTRAINT_VIOLATION;
	}
	talloc_steal(list, dn_key);

	ret = ldb_kv_dn_list_load(module, ldb_kv, dn_key, list);
	if (ret != LDB_SUCCESS && ret != LDB_ERR_NO_SUCH_OBJECT) {
		talloc_free(list);
		return ret;
	}

	/*
	 * Check for duplicates in the @IDXDN DN -> GUID record
	 *
	 * This is very normal, it just means a duplicate DN creation
	 * was attempted, so don't set the error string or print scary
	 * messages.
	 */
	if (list->count > 0 &&
	    ldb_attr_cmp(el->name, LDB_KV_IDXDN) == 0 &&
	    truncation == KEY_NOT_TRUNCATED) {

		talloc_free(list);
		return LDB_ERR_CONSTRAINT_VIOLATION;

	} else if (list->count > 0
		   && ldb_attr_cmp(el->name, LDB_KV_IDXDN) == 0) {

		/*
		 * At least one existing entry in the DN->GUID index, which
		 * arises when the DN indexes have been truncated
		 *
		 * So need to pull the DN's to check if it's really a duplicate
		 */
		int i;
		for (i=0; i < list->count; i++) {
			uint8_t guid_key[LDB_KV_GUID_KEY_SIZE];
			struct ldb_val key = {
				.data = guid_key,
				.length = sizeof(guid_key)
			};
			const int flags = LDB_UNPACK_DATA_FLAG_NO_ATTRS;
			struct ldb_message *rec = ldb_msg_new(ldb);
			if (rec == NULL) {
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
				talloc_free(rec);
				continue;
			}

			if (ret != LDB_SUCCESS) {
				/* an internal error */
				TALLOC_FREE(rec);
				TALLOC_FREE(list);
				return LDB_ERR_OPERATIONS_ERROR;
			}
			/*
			 * The DN we are trying to add to the DB and index
			 * is already here, so we must deny the addition
			 */
			if (ldb_dn_compare(msg->dn, rec->dn) == 0) {
				TALLOC_FREE(rec);
				TALLOC_FREE(list);
				return LDB_ERR_CONSTRAINT_VIOLATION;
			}
		}
	}

	/*
	 * Check for duplicates in unique indexes
	 *
	 * We don't need to do a loop test like the @IDXDN case
	 * above as we have a ban on long unique index values
	 * at the start of this function.
	 */
	if (list->count > 0 &&
	    ((a != NULL
	      && (a->flags & LDB_ATTR_FLAG_UNIQUE_INDEX ||
		  (el->flags & LDB_FLAG_INTERNAL_FORCE_UNIQUE_INDEX))))) {
		/*
		 * We do not want to print info about a possibly
		 * confidential DN that the conflict was with in the
		 * user-visible error string
		 */

		if (ldb_kv->cache->GUID_index_attribute == NULL) {
			ldb_debug(ldb, LDB_DEBUG_WARNING,
				  __location__
				  ": unique index violation on %s in %s, "
				  "conficts with %*.*s in %s",
				  el->name, ldb_dn_get_linearized(msg->dn),
				  (int)list->dn[0].length,
				  (int)list->dn[0].length,
				  list->dn[0].data,
				  ldb_dn_get_linearized(dn_key));
		} else {
			/* This can't fail, gives a default at worst */
			const struct ldb_schema_attribute *attr =
			    ldb_schema_attribute_by_name(
				ldb, ldb_kv->cache->GUID_index_attribute);
			struct ldb_val v;
			ret = attr->syntax->ldif_write_fn(ldb, list,
							  &list->dn[0], &v);
			if (ret == LDB_SUCCESS) {
				ldb_debug(ldb,
					  LDB_DEBUG_WARNING,
					  __location__
					  ": unique index violation on %s in "
					  "%s, conficts with %s %*.*s in %s",
					  el->name,
					  ldb_dn_get_linearized(msg->dn),
					  ldb_kv->cache->GUID_index_attribute,
					  (int)v.length,
					  (int)v.length,
					  v.data,
					  ldb_dn_get_linearized(dn_key));
			}
		}
		ldb_asprintf_errstring(ldb,
				       __location__ ": unique index violation "
				       "on %s in %s",
				       el->name,
				       ldb_dn_get_linearized(msg->dn));
		talloc_free(list);
		return LDB_ERR_CONSTRAINT_VIOLATION;
	}

	/* overallocate the list a bit, to reduce the number of
	 * realloc trigered copies */
	alloc_len = ((list->count+1)+7) & ~7;
	list->dn = talloc_realloc(list, list->dn, struct ldb_val, alloc_len);
	if (list->dn == NULL) {
		talloc_free(list);
		return LDB_ERR_OPERATIONS_ERROR;
	}

	if (ldb_kv->cache->GUID_index_attribute == NULL) {
		const char *dn_str = ldb_dn_get_linearized(msg->dn);
		list->dn[list->count].data
			= (uint8_t *)talloc_strdup(list->dn, dn_str);
		if (list->dn[list->count].data == NULL) {
			talloc_free(list);
			return LDB_ERR_OPERATIONS_ERROR;
		}
		list->dn[list->count].length = strlen(dn_str);
	} else {
		const struct ldb_val *key_val;
		struct ldb_val *exact = NULL, *next = NULL;
		key_val = ldb_msg_find_ldb_val(
		    msg, ldb_kv->cache->GUID_index_attribute);
		if (key_val == NULL) {
			talloc_free(list);
			return ldb_module_operr(module);
		}

		if (key_val->length != LDB_KV_GUID_SIZE) {
			talloc_free(list);
			return ldb_module_operr(module);
		}

		BINARY_ARRAY_SEARCH_GTE(list->dn, list->count,
					*key_val, ldb_val_equal_exact_ordered,
					exact, next);

		/*
		 * Give a warning rather than fail, this could be a
		 * duplicate value in the record allowed by a caller
		 * forcing in the value with
		 * LDB_FLAG_INTERNAL_DISABLE_SINGLE_VALUE_CHECK
		 */
		if (exact != NULL && truncation == KEY_NOT_TRUNCATED) {
			/* This can't fail, gives a default at worst */
			const struct ldb_schema_attribute *attr =
			    ldb_schema_attribute_by_name(
				ldb, ldb_kv->cache->GUID_index_attribute);
			struct ldb_val v;
			ret = attr->syntax->ldif_write_fn(ldb, list,
							  exact, &v);
			if (ret == LDB_SUCCESS) {
				ldb_debug(ldb,
					  LDB_DEBUG_WARNING,
					  __location__
					  ": duplicate attribute value in %s "
					  "for index on %s, "
					  "duplicate of %s %*.*s in %s",
					  ldb_dn_get_linearized(msg->dn),
					  el->name,
					  ldb_kv->cache->GUID_index_attribute,
					  (int)v.length,
					  (int)v.length,
					  v.data,
					  ldb_dn_get_linearized(dn_key));
			}
		}

		if (next == NULL) {
			next = &list->dn[list->count];
		} else {
			memmove(&next[1], next,
				sizeof(*next) * (list->count - (next - list->dn)));
		}
		*next = ldb_val_dup(list->dn, key_val);
		if (next->data == NULL) {
			talloc_free(list);
			return ldb_module_operr(module);
		}
	}
	list->count++;

	ret = ldb_kv_dn_list_store(module, dn_key, list);

	talloc_free(list);

	return ret;
}