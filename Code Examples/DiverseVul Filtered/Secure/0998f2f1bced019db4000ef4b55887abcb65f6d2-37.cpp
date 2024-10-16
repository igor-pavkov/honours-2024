static unsigned ldb_kv_max_key_length(struct ldb_kv_private *ldb_kv)
{
	if (ldb_kv->max_key_length == 0) {
		return UINT_MAX;
	}
	return ldb_kv->max_key_length;
}