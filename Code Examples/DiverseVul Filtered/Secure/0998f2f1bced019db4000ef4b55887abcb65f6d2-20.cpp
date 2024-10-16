static bool list_intersect(struct ldb_context *ldb,
			   struct ldb_kv_private *ldb_kv,
			   struct dn_list *list,
			   const struct dn_list *list2)
{
	const struct dn_list *short_list, *long_list;
	struct dn_list *list3;
	unsigned int i;

	if (list->count == 0) {
		/* 0 & X == 0 */
		return true;
	}
	if (list2->count == 0) {
		/* X & 0 == 0 */
		list->count = 0;
		list->dn = NULL;
		return true;
	}

	/* the indexing code is allowed to return a longer list than
	   what really matches, as all results are filtered by the
	   full expression at the end - this shortcut avoids a lot of
	   work in some cases */
	if (list->count < 2 && list2->count > 10 && list2->strict == false) {
		return true;
	}
	if (list2->count < 2 && list->count > 10 && list->strict == false) {
		list->count = list2->count;
		list->dn = list2->dn;
		/* note that list2 may not be the parent of list2->dn,
		   as list2->dn may be owned by ltdb->idxptr. In that
		   case we expect this reparent call to fail, which is
		   OK */
		talloc_reparent(list2, list, list2->dn);
		return true;
	}

	if (list->count > list2->count) {
		short_list = list2;
		long_list = list;
	} else {
		short_list = list;
		long_list = list2;
	}

	list3 = talloc_zero(list, struct dn_list);
	if (list3 == NULL) {
		return false;
	}

	list3->dn = talloc_array(list3, struct ldb_val,
				 MIN(list->count, list2->count));
	if (!list3->dn) {
		talloc_free(list3);
		return false;
	}
	list3->count = 0;

	for (i=0;i<short_list->count;i++) {
		/* For the GUID index case, this is a binary search */
		if (ldb_kv_dn_list_find_val(
			ldb_kv, long_list, &short_list->dn[i]) != -1) {
			list3->dn[list3->count] = short_list->dn[i];
			list3->count++;
		}
	}

	list->strict |= list2->strict;
	list->dn = talloc_steal(list, list3->dn);
	list->count = list3->count;
	talloc_free(list3);

	return true;
}