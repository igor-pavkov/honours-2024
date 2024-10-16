static int ldb_kv_dn_list_load(struct ldb_module *module,
			       struct ldb_kv_private *ldb_kv,
			       struct ldb_dn *dn,
			       struct dn_list *list)
{
	struct ldb_message *msg;
	int ret, version;
	struct ldb_message_element *el;
	TDB_DATA rec;
	struct dn_list *list2;
	TDB_DATA key;

	list->dn = NULL;
	list->count = 0;

	/* see if we have any in-memory index entries */
	if (ldb_kv->idxptr == NULL || ldb_kv->idxptr->itdb == NULL) {
		goto normal_index;
	}

	key.dptr = discard_const_p(unsigned char, ldb_dn_get_linearized(dn));
	key.dsize = strlen((char *)key.dptr);

	rec = tdb_fetch(ldb_kv->idxptr->itdb, key);
	if (rec.dptr == NULL) {
		goto normal_index;
	}

	/* we've found an in-memory index entry */
	list2 = ldb_kv_index_idxptr(module, rec, true);
	if (list2 == NULL) {
		free(rec.dptr);
		return LDB_ERR_OPERATIONS_ERROR;
	}
	free(rec.dptr);

	*list = *list2;
	return LDB_SUCCESS;

normal_index:
	msg = ldb_msg_new(list);
	if (msg == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	ret = ldb_kv_search_dn1(module,
				dn,
				msg,
				LDB_UNPACK_DATA_FLAG_NO_DATA_ALLOC |
				    LDB_UNPACK_DATA_FLAG_NO_DN);
	if (ret != LDB_SUCCESS) {
		talloc_free(msg);
		return ret;
	}

	el = ldb_msg_find_element(msg, LDB_KV_IDX);
	if (!el) {
		talloc_free(msg);
		return LDB_SUCCESS;
	}

	version = ldb_msg_find_attr_as_int(msg, LDB_KV_IDXVERSION, 0);

	/*
	 * we avoid copying the strings by stealing the list.  We have
	 * to steal msg onto el->values (which looks odd) because we
	 * asked for the memory to be allocated on msg, not on each
	 * value with LDB_UNPACK_DATA_FLAG_NO_DATA_ALLOC above
	 */
	if (ldb_kv->cache->GUID_index_attribute == NULL) {
		/* check indexing version number */
		if (version != LDB_KV_INDEXING_VERSION) {
			ldb_debug_set(ldb_module_get_ctx(module),
				      LDB_DEBUG_ERROR,
				      "Wrong DN index version %d "
				      "expected %d for %s",
				      version, LDB_KV_INDEXING_VERSION,
				      ldb_dn_get_linearized(dn));
			talloc_free(msg);
			return LDB_ERR_OPERATIONS_ERROR;
		}

		talloc_steal(el->values, msg);
		list->dn = talloc_steal(list, el->values);
		list->count = el->num_values;
	} else {
		unsigned int i;
		if (version != LDB_KV_GUID_INDEXING_VERSION) {
			/* This is quite likely during the DB startup
			   on first upgrade to using a GUID index */
			ldb_debug_set(ldb_module_get_ctx(module),
				      LDB_DEBUG_ERROR,
				      "Wrong GUID index version %d "
				      "expected %d for %s",
				      version, LDB_KV_GUID_INDEXING_VERSION,
				      ldb_dn_get_linearized(dn));
			talloc_free(msg);
			return LDB_ERR_OPERATIONS_ERROR;
		}

		if (el->num_values == 0) {
			talloc_free(msg);
			return LDB_ERR_OPERATIONS_ERROR;
		}

		if ((el->values[0].length % LDB_KV_GUID_SIZE) != 0) {
			talloc_free(msg);
			return LDB_ERR_OPERATIONS_ERROR;
		}

		list->count = el->values[0].length / LDB_KV_GUID_SIZE;
		list->dn = talloc_array(list, struct ldb_val, list->count);
		if (list->dn == NULL) {
			talloc_free(msg);
			return LDB_ERR_OPERATIONS_ERROR;
		}

		/*
		 * The actual data is on msg, due to
		 * LDB_UNPACK_DATA_FLAG_NO_DATA_ALLOC
		 */
		talloc_steal(list->dn, msg);
		for (i = 0; i < list->count; i++) {
			list->dn[i].data
				= &el->values[0].data[i * LDB_KV_GUID_SIZE];
			list->dn[i].length = LDB_KV_GUID_SIZE;
		}
	}

	/* We don't need msg->elements any more */
	talloc_free(msg->elements);
	return LDB_SUCCESS;
}