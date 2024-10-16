static void ldb_kv_dn_list_sort(struct ldb_kv_private *ltdb,
				struct dn_list *list)
{
	if (list->count < 2) {
		return;
	}

	/* We know the list is sorted when using the GUID index */
	if (ltdb->cache->GUID_index_attribute != NULL) {
		return;
	}

	TYPESAFE_QSORT(list->dn, list->count,
		       ldb_val_equal_exact_for_qsort);
}