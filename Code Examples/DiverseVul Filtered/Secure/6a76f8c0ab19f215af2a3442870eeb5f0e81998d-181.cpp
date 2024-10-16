static void free_ftrace_hash(struct ftrace_hash *hash)
{
	if (!hash || hash == EMPTY_HASH)
		return;
	ftrace_hash_clear(hash);
	kfree(hash->buckets);
	kfree(hash);
}