static int re_index(struct ldb_kv_private *ldb_kv,
		    struct ldb_val key,
		    struct ldb_val val,
		    void *state)
{
	struct ldb_context *ldb;
	struct ldb_kv_reindex_context *ctx =
	    (struct ldb_kv_reindex_context *)state;
	struct ldb_module *module = ctx->module;
	struct ldb_message *msg;
	unsigned int nb_elements_in_db;
	int ret;
	bool is_record;

	ldb = ldb_module_get_ctx(module);

	if (key.length > 4 &&
	    memcmp(key.data, "DN=@", 4) == 0) {
		return 0;
	}

	is_record = ldb_kv_key_is_record(key);
	if (is_record == false) {
		return 0;
	}

	msg = ldb_msg_new(module);
	if (msg == NULL) {
		return -1;
	}

	ret = ldb_unpack_data_only_attr_list_flags(ldb, &val,
						   msg,
						   NULL, 0,
						   LDB_UNPACK_DATA_FLAG_NO_DATA_ALLOC,
						   &nb_elements_in_db);
	if (ret != 0) {
		ldb_debug(ldb, LDB_DEBUG_ERROR, "Invalid data for index %s\n",
						ldb_dn_get_linearized(msg->dn));
		ctx->error = ret;
		talloc_free(msg);
		return -1;
	}

	if (msg->dn == NULL) {
		ldb_debug(ldb, LDB_DEBUG_ERROR,
			  "Refusing to re-index as GUID "
			  "key %*.*s with no DN\n",
			  (int)key.length, (int)key.length,
			  (char *)key.data);
		talloc_free(msg);
		return -1;
	}

	ret = ldb_kv_index_onelevel(module, msg, 1);
	if (ret != LDB_SUCCESS) {
		ldb_debug(ldb, LDB_DEBUG_ERROR,
			  "Adding special ONE LEVEL index failed (%s)!",
						ldb_dn_get_linearized(msg->dn));
		talloc_free(msg);
		return -1;
	}

	ret = ldb_kv_index_add_all(module, ldb_kv, msg);

	if (ret != LDB_SUCCESS) {
		ctx->error = ret;
		talloc_free(msg);
		return -1;
	}

	talloc_free(msg);

	ctx->count++;
	if (ctx->count % 10000 == 0) {
		ldb_debug(ldb, LDB_DEBUG_WARNING,
			  "Reindexing: re-indexed %u records so far",
			  ctx->count);
	}

	return 0;
}