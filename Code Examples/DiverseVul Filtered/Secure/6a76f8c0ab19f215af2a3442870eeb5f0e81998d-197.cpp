static void ftrace_filter_reset(struct ftrace_hash *hash)
{
	mutex_lock(&ftrace_lock);
	ftrace_hash_clear(hash);
	mutex_unlock(&ftrace_lock);
}