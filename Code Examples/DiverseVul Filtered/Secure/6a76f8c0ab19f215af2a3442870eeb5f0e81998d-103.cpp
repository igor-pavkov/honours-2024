static struct ftrace_hash *alloc_ftrace_hash(int size_bits)
{
	struct ftrace_hash *hash;
	int size;

	hash = kzalloc(sizeof(*hash), GFP_KERNEL);
	if (!hash)
		return NULL;

	size = 1 << size_bits;
	hash->buckets = kcalloc(size, sizeof(*hash->buckets), GFP_KERNEL);

	if (!hash->buckets) {
		kfree(hash);
		return NULL;
	}

	hash->size_bits = size_bits;

	return hash;
}