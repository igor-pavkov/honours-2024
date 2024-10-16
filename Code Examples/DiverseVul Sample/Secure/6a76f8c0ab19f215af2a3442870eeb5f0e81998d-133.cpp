static void add_ftrace_list_ops(struct ftrace_ops **list,
				struct ftrace_ops *main_ops,
				struct ftrace_ops *ops)
{
	int first = *list == &ftrace_list_end;
	add_ftrace_ops(list, ops);
	if (first)
		add_ftrace_ops(&ftrace_ops_list, main_ops);
}