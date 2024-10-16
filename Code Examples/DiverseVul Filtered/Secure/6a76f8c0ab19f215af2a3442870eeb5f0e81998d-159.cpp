static void ftrace_hash_clear(struct ftrace_hash *hash)
{
	struct hlist_head *hhd;
	struct hlist_node *tn;
	struct ftrace_func_entry *entry;
	int size = 1 << hash->size_bits;
	int i;

	if (!hash->count)
		return;

	for (i = 0; i < size; i++) {
		hhd = &hash->buckets[i];
		hlist_for_each_entry_safe(entry, tn, hhd, hlist)
			free_hash_entry(hash, entry);
	}
	FTRACE_WARN_ON(hash->count);
}