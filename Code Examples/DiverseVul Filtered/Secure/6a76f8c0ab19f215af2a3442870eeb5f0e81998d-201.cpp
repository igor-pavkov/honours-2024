void ftrace_free_filter(struct ftrace_ops *ops)
{
	free_ftrace_hash(ops->filter_hash);
	free_ftrace_hash(ops->notrace_hash);
}