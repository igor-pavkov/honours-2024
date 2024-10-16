static int ftrace_function_set_regexp(struct ftrace_ops *ops, int filter,
				      int reset, char *re, int len)
{
	int ret;

	if (filter)
		ret = ftrace_set_filter(ops, re, len, reset);
	else
		ret = ftrace_set_notrace(ops, re, len, reset);

	return ret;
}