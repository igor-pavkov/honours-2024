static int remove_ftrace_list_ops(struct ftrace_ops **list,
				  struct ftrace_ops *main_ops,
				  struct ftrace_ops *ops)
{
	int ret = remove_ftrace_ops(list, ops);
	if (!ret && *list == &ftrace_list_end)
		ret = remove_ftrace_ops(&ftrace_ops_list, main_ops);
	return ret;
}