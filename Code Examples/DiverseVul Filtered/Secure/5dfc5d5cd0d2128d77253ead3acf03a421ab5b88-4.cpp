static void hpack_dht_dump(FILE *out, const struct hpack_dht *dht)
{
	unsigned int i;
	unsigned int slot;
	char name[4096], value[4096];

	for (i = HPACK_SHT_SIZE; i < HPACK_SHT_SIZE + dht->used; i++) {
		slot = (hpack_get_dte(dht, i - HPACK_SHT_SIZE + 1) - dht->dte);
		fprintf(out, "idx=%d slot=%u name=<%s> value=<%s> addr=%u-%u\n",
			i, slot,
			istpad(name, hpack_idx_to_name(dht, i)).ptr,
			istpad(value, hpack_idx_to_value(dht, i)).ptr,
			dht->dte[slot].addr, dht->dte[slot].addr+dht->dte[slot].nlen+dht->dte[slot].vlen-1);
	}
}