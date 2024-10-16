static int ops_traces_mod(struct ftrace_ops *ops)
{
	struct ftrace_hash *hash;

	hash = ops->filter_hash;
	return ftrace_hash_empty(hash);
}