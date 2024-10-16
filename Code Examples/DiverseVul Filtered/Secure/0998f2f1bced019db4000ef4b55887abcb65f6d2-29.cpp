static int ldb_kv_dn_list_find_val(struct ldb_kv_private *ldb_kv,
				   const struct dn_list *list,
				   const struct ldb_val *v)
{
	unsigned int i;
	struct ldb_val *exact = NULL, *next = NULL;

	if (ldb_kv->cache->GUID_index_attribute == NULL) {
		for (i=0; i<list->count; i++) {
			if (ldb_val_equal_exact(&list->dn[i], v) == 1) {
				return i;
			}
		}
		return -1;
	}

	BINARY_ARRAY_SEARCH_GTE(list->dn, list->count,
				*v, ldb_val_equal_exact_ordered,
				exact, next);
	if (exact == NULL) {
		return -1;
	}
	/* Not required, but keeps the compiler quiet */
	if (next != NULL) {
		return -1;
	}

	i = exact - list->dn;
	return i;
}