int ftrace_set_filter_ip(struct ftrace_ops *ops, unsigned long ip,
			 int remove, int reset)
{
	return ftrace_set_addr(ops, ip, remove, reset, 1);
}