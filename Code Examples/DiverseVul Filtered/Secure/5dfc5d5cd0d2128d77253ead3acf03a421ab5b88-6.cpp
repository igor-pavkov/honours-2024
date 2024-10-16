int __hpack_dht_make_room(struct hpack_dht *dht, unsigned int needed)
{
	unsigned int used = dht->used;
	unsigned int wrap = dht->wrap;
	unsigned int tail;

	do {
		tail = ((dht->head + 1U < used) ? wrap : 0) + dht->head + 1U - used;
		dht->total -= dht->dte[tail].nlen + dht->dte[tail].vlen;
		if (tail == dht->front)
			dht->front = dht->head;
		used--;
	} while (used && used * 32 + dht->total + needed + 32 > dht->size);

	dht->used = used;

	/* realign if empty */
	if (!used)
		dht->front = dht->head = 0;

	/* pack the table if it doesn't wrap anymore */
	if (dht->head + 1U >= used)
		dht->wrap = dht->head + 1;

	/* no need to check for 'used' here as if it doesn't fit, used==0 */
	return needed + 32 <= dht->size;
}