static void hpack_dht_check_consistency(const struct hpack_dht *dht)
{
	unsigned slot = hpack_dht_get_tail(dht);
	unsigned used2 = dht->used;
	unsigned total = 0;

	if (!dht->used)
		return;

	if (dht->front >= dht->wrap)
		abort();

	if (dht->used > dht->wrap)
		abort();

	if (dht->head >= dht->wrap)
		abort();

	while (used2--) {
		total += dht->dte[slot].nlen + dht->dte[slot].vlen;
		slot++;
		if (slot >= dht->wrap)
			slot = 0;
	}

	if (total != dht->total) {
		fprintf(stderr, "%d: total=%u dht=%u\n", __LINE__, total, dht->total);
		abort();
	}
}