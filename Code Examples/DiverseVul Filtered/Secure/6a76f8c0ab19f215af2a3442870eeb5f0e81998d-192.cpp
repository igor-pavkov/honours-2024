static void __add_hash_entry(struct ftrace_hash *hash,
			     struct ftrace_func_entry *entry)
{
	struct hlist_head *hhd;
	unsigned long key;

	if (hash->size_bits)
		key = hash_long(entry->ip, hash->size_bits);
	else
		key = 0;

	hhd = &hash->buckets[key];
	hlist_add_head(&entry->hlist, hhd);
	hash->count++;
}