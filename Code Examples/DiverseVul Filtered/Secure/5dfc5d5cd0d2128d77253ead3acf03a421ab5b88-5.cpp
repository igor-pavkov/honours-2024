static struct hpack_dht *hpack_dht_defrag(struct hpack_dht *dht)
{
	struct hpack_dht *alt_dht;
	uint16_t old, new;
	uint32_t addr;

	/* Note: for small tables we could use alloca() instead but
	 * portability especially for large tables can be problematic.
	 */
	alt_dht = hpack_dht_alloc(dht->size);
	if (!alt_dht)
		return NULL;

	alt_dht->total = dht->total;
	alt_dht->used = dht->used;
	alt_dht->wrap = dht->used;

	new = 0;
	addr = alt_dht->size;

	if (dht->used) {
		/* start from the tail */
		old = hpack_dht_get_tail(dht);
		do {
			alt_dht->dte[new].nlen = dht->dte[old].nlen;
			alt_dht->dte[new].vlen = dht->dte[old].vlen;
			addr -= dht->dte[old].nlen + dht->dte[old].vlen;
			alt_dht->dte[new].addr = addr;

			memcpy((void *)alt_dht + alt_dht->dte[new].addr,
			       (void *)dht + dht->dte[old].addr,
			       dht->dte[old].nlen + dht->dte[old].vlen);

			old++;
			if (old >= dht->wrap)
				old = 0;
			new++;
		} while (new < dht->used);
	}

	alt_dht->front = alt_dht->head = new - 1;

	memcpy(dht, alt_dht, dht->size);
	hpack_dht_free(alt_dht);

	return dht;
}