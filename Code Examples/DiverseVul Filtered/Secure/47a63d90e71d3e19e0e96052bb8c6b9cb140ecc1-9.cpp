static int64 xattr_lookup_hash(const item_list *xalp)
{
	const rsync_xa *rxas = xalp->items;
	size_t i;
	int64 key = hashlittle(&xalp->count, sizeof xalp->count);

	for (i = 0; i < xalp->count; i++) {
		key += hashlittle(rxas[i].name, rxas[i].name_len);
		if (rxas[i].datum_len > MAX_FULL_DATUM)
			key += hashlittle(rxas[i].datum, MAX_DIGEST_LEN);
		else
			key += hashlittle(rxas[i].datum, rxas[i].datum_len);
	}

	if (key == 0) {
		/* This is very unlikely, but we should never
		 * return 0 as hashtable_find() doesn't like it. */
		return 1;
	}

	return key;
}