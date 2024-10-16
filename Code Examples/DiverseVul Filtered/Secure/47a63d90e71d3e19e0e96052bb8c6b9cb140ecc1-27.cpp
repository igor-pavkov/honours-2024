static int find_matching_xattr(const item_list *xalp)
{
	const struct ht_int64_node *node;
	const rsync_xa_list_ref *ref;
	int64 key;

	if (rsync_xal_h == NULL)
		return -1;

	key = xattr_lookup_hash(xalp);

	node = hashtable_find(rsync_xal_h, key, 0);
	if (node == NULL)
		return -1;

	if (node->data == NULL)
		return -1;

	for (ref = node->data; ref != NULL; ref = ref->next) {
		const rsync_xa_list *ptr = rsync_xal_l.items;
		const rsync_xa *rxas1;
		const rsync_xa *rxas2 = xalp->items;
		size_t j;

		ptr += ref->ndx;
		rxas1 = ptr->xa_items.items;

		/* Wrong number of elements? */
		if (ptr->xa_items.count != xalp->count)
			continue;
		/* any elements different? */
		for (j = 0; j < xalp->count; j++) {
			if (rxas1[j].name_len != rxas2[j].name_len
			 || rxas1[j].datum_len != rxas2[j].datum_len
			 || strcmp(rxas1[j].name, rxas2[j].name))
				break;
			if (rxas1[j].datum_len > MAX_FULL_DATUM) {
				if (memcmp(rxas1[j].datum + 1,
					   rxas2[j].datum + 1,
					   MAX_DIGEST_LEN) != 0)
					break;
			} else {
				if (memcmp(rxas1[j].datum, rxas2[j].datum,
					   rxas2[j].datum_len))
					break;
			}
		}
		/* no differences found.  This is The One! */
		if (j == xalp->count)
			return ref->ndx;
	}

	return -1;
}