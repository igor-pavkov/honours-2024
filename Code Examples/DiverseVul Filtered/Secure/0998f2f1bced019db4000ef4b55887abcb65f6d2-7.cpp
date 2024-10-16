static int ldb_kv_dn_list_find_msg(struct ldb_kv_private *ldb_kv,
				   struct dn_list *list,
				   const struct ldb_message *msg)
{
	struct ldb_val v;
	const struct ldb_val *key_val;
	if (ldb_kv->cache->GUID_index_attribute == NULL) {
		const char *dn_str = ldb_dn_get_linearized(msg->dn);
		v.data = discard_const_p(unsigned char, dn_str);
		v.length = strlen(dn_str);
	} else {
		key_val = ldb_msg_find_ldb_val(
		    msg, ldb_kv->cache->GUID_index_attribute);
		if (key_val == NULL) {
			return -1;
		}
		v = *key_val;
	}
	return ldb_kv_dn_list_find_val(ldb_kv, list, &v);
}