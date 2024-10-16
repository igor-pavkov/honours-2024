static int ldb_kv_modify_index_dn(struct ldb_module *module,
				  struct ldb_kv_private *ldb_kv,
				  const struct ldb_message *msg,
				  struct ldb_dn *dn,
				  const char *index,
				  int add)
{
	struct ldb_message_element el;
	struct ldb_val val;
	int ret;

	val.data = (uint8_t *)((uintptr_t)ldb_dn_get_casefold(dn));
	if (val.data == NULL) {
		const char *dn_str = ldb_dn_get_linearized(dn);
		ldb_asprintf_errstring(ldb_module_get_ctx(module),
				       __location__ ": Failed to modify %s "
						    "against %s in %s: failed "
						    "to get casefold DN",
				       index,
				       ldb_kv->cache->GUID_index_attribute,
				       dn_str);
		return LDB_ERR_OPERATIONS_ERROR;
	}

	val.length = strlen((char *)val.data);
	el.name = index;
	el.values = &val;
	el.num_values = 1;

	if (add) {
		ret = ldb_kv_index_add1(module, ldb_kv, msg, &el, 0);
	} else { /* delete */
		ret = ldb_kv_index_del_value(module, ldb_kv, msg, &el, 0);
	}

	if (ret != LDB_SUCCESS) {
		struct ldb_context *ldb = ldb_module_get_ctx(module);
		const char *dn_str = ldb_dn_get_linearized(dn);
		ldb_asprintf_errstring(ldb,
				       __location__ ": Failed to modify %s "
						    "against %s in %s - %s",
				       index,
				       ldb_kv->cache->GUID_index_attribute,
				       dn_str,
				       ldb_errstring(ldb));
		return ret;
	}
	return ret;
}