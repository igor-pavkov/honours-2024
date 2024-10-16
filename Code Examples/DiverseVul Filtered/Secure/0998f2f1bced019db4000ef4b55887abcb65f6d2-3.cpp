int ldb_kv_index_del_value(struct ldb_module *module,
			   struct ldb_kv_private *ldb_kv,
			   const struct ldb_message *msg,
			   struct ldb_message_element *el,
			   unsigned int v_idx)
{
	struct ldb_context *ldb;
	struct ldb_dn *dn_key;
	const char *dn_str;
	int ret, i;
	unsigned int j;
	struct dn_list *list;
	struct ldb_dn *dn = msg->dn;
	enum key_truncation truncation = KEY_NOT_TRUNCATED;

	ldb = ldb_module_get_ctx(module);

	dn_str = ldb_dn_get_linearized(dn);
	if (dn_str == NULL) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	if (dn_str[0] == '@') {
		return LDB_SUCCESS;
	}

	dn_key = ldb_kv_index_key(
	    ldb, ldb_kv, el->name, &el->values[v_idx], NULL, &truncation);
	/*
	 * We ignore key truncation in ltdb_index_add1() so
	 * match that by ignoring it here as well
	 *
	 * Multiple values are legitimate and accepted
	 */
	if (!dn_key) {
		return LDB_ERR_OPERATIONS_ERROR;
	}

	list = talloc_zero(dn_key, struct dn_list);
	if (list == NULL) {
		talloc_free(dn_key);
		return LDB_ERR_OPERATIONS_ERROR;
	}

	ret = ldb_kv_dn_list_load(module, ldb_kv, dn_key, list);
	if (ret == LDB_ERR_NO_SUCH_OBJECT) {
		/* it wasn't indexed. Did we have an earlier error? If we did then
		   its gone now */
		talloc_free(dn_key);
		return LDB_SUCCESS;
	}

	if (ret != LDB_SUCCESS) {
		talloc_free(dn_key);
		return ret;
	}

	/*
	 * Find one of the values matching this message to remove
	 */
	i = ldb_kv_dn_list_find_msg(ldb_kv, list, msg);
	if (i == -1) {
		/* nothing to delete */
		talloc_free(dn_key);
		return LDB_SUCCESS;
	}

	j = (unsigned int) i;
	if (j != list->count - 1) {
		memmove(&list->dn[j], &list->dn[j+1], sizeof(list->dn[0])*(list->count - (j+1)));
	}
	list->count--;
	if (list->count == 0) {
		talloc_free(list->dn);
		list->dn = NULL;
	} else {
		list->dn = talloc_realloc(list, list->dn, struct ldb_val, list->count);
	}

	ret = ldb_kv_dn_list_store(module, dn_key, list);

	talloc_free(dn_key);

	return ret;
}