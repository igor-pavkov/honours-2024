static int ldb_kv_index_dn_base_dn(struct ldb_module *module,
				   struct ldb_kv_private *ldb_kv,
				   struct ldb_dn *base_dn,
				   struct dn_list *dn_list,
				   enum key_truncation *truncation)
{
	const struct ldb_val *guid_val = NULL;
	if (ldb_kv->cache->GUID_index_attribute == NULL) {
		dn_list->dn = talloc_array(dn_list, struct ldb_val, 1);
		if (dn_list->dn == NULL) {
			return ldb_module_oom(module);
		}
		dn_list->dn[0].data = discard_const_p(unsigned char,
						      ldb_dn_get_linearized(base_dn));
		if (dn_list->dn[0].data == NULL) {
			return ldb_module_oom(module);
		}
		dn_list->dn[0].length = strlen((char *)dn_list->dn[0].data);
		dn_list->count = 1;

		return LDB_SUCCESS;
	}

	if (ldb_kv->cache->GUID_index_dn_component != NULL) {
		guid_val = ldb_dn_get_extended_component(
		    base_dn, ldb_kv->cache->GUID_index_dn_component);
	}

	if (guid_val != NULL) {
		dn_list->dn = talloc_array(dn_list, struct ldb_val, 1);
		if (dn_list->dn == NULL) {
			return ldb_module_oom(module);
		}
		dn_list->dn[0].data = guid_val->data;
		dn_list->dn[0].length = guid_val->length;
		dn_list->count = 1;

		return LDB_SUCCESS;
	}

	return ldb_kv_index_dn_attr(
	    module, ldb_kv, LDB_KV_IDXDN, base_dn, dn_list, truncation);
}