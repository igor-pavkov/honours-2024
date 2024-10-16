static int add_hash_entry(struct ftrace_hash *hash, unsigned long ip)
{
	struct ftrace_func_entry *entry;

	entry = kmalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;

	entry->ip = ip;
	__add_hash_entry(hash, entry);

	return 0;
}