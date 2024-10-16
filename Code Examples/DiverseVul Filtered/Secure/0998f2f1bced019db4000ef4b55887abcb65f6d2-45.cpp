static bool list_union(struct ldb_context *ldb,
		       struct ldb_kv_private *ldb_kv,
		       struct dn_list *list,
		       struct dn_list *list2)
{
	struct ldb_val *dn3;
	unsigned int i = 0, j = 0, k = 0;

	if (list2->count == 0) {
		/* X | 0 == X */
		return true;
	}

	if (list->count == 0) {
		/* 0 | X == X */
		list->count = list2->count;
		list->dn = list2->dn;
		/* note that list2 may not be the parent of list2->dn,
		   as list2->dn may be owned by ltdb->idxptr. In that
		   case we expect this reparent call to fail, which is
		   OK */
		talloc_reparent(list2, list, list2->dn);
		return true;
	}

	/*
	 * Sort the lists (if not in GUID DN mode) so we can do
	 * the de-duplication during the merge
	 *
	 * NOTE: This can sort the in-memory index values, as list or
	 * list2 might not be a copy!
	 */
	ldb_kv_dn_list_sort(ldb_kv, list);
	ldb_kv_dn_list_sort(ldb_kv, list2);

	dn3 = talloc_array(list, struct ldb_val, list->count + list2->count);
	if (!dn3) {
		ldb_oom(ldb);
		return false;
	}

	while (i < list->count || j < list2->count) {
		int cmp;
		if (i >= list->count) {
			cmp = 1;
		} else if (j >= list2->count) {
			cmp = -1;
		} else {
			cmp = ldb_val_equal_exact_ordered(list->dn[i],
							  &list2->dn[j]);
		}

		if (cmp < 0) {
			/* Take list */
			dn3[k] = list->dn[i];
			i++;
			k++;
		} else if (cmp > 0) {
			/* Take list2 */
			dn3[k] = list2->dn[j];
			j++;
			k++;
		} else {
			/* Equal, take list */
			dn3[k] = list->dn[i];
			i++;
			j++;
			k++;
		}
	}

	list->dn = dn3;
	list->count = k;

	return true;
}