static void ftrace_hash_rec_enable(struct ftrace_ops *ops,
				   int filter_hash)
{
	__ftrace_hash_rec_update(ops, filter_hash, 1);
}