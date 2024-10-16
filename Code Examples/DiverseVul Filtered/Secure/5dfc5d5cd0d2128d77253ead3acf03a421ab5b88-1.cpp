static inline unsigned int hpack_dht_get_tail(const struct hpack_dht *dht)
{
	return ((dht->head + 1U < dht->used) ? dht->wrap : 0) + dht->head + 1U - dht->used;
}