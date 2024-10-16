static bool ftrace_hash_empty(struct ftrace_hash *hash)
{
	return !hash || !hash->count;
}