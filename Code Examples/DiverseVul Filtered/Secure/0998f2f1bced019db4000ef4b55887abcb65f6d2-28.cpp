static bool ldb_kv_index_unique(struct ldb_context *ldb,
				struct ldb_kv_private *ldb_kv,
				const char *attr)
{
	const struct ldb_schema_attribute *a;
	if (ldb_kv->cache->GUID_index_attribute != NULL) {
		if (ldb_attr_cmp(attr, ldb_kv->cache->GUID_index_attribute) ==
		    0) {
			return true;
		}
	}
	if (ldb_attr_dn(attr) == 0) {
		return true;
	}

	a = ldb_schema_attribute_by_name(ldb, attr);
	if (a->flags & LDB_ATTR_FLAG_UNIQUE_INDEX) {
		return true;
	}
	return false;
}