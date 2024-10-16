static int re_key(struct ldb_kv_private *ldb_kv,
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
	struct ldb_val key2;
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

	/* check if the DN key has changed, perhaps due to the case
	   insensitivity of an element changing, or a change from DN
	   to GUID keys */
	key2 = ldb_kv_key_msg(module, msg, msg);
	if (key2.data == NULL) {
		/* probably a corrupt record ... darn */
		ldb_debug(ldb, LDB_DEBUG_ERROR, "Invalid DN in re_index: %s",
						ldb_dn_get_linearized(msg->dn));
		talloc_free(msg);
		return 0;
	}
	if (key.length != key2.length ||
	    (memcmp(key.data, key2.data, key.length) != 0)) {
		ldb_kv->kv_ops->update_in_iterate(
		    ldb_kv, key, key2, val, ctx);
	}
	talloc_free(key2.data);

	talloc_free(msg);

	ctx->count++;
	if (ctx->count % 10000 == 0) {
		ldb_debug(ldb, LDB_DEBUG_WARNING,
			  "Reindexing: re-keyed %u records so far",
			  ctx->count);
	}

	return 0;
}