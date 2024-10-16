void ftrace_modify_all_code(int command)
{
	if (command & FTRACE_UPDATE_CALLS)
		ftrace_replace_code(1);
	else if (command & FTRACE_DISABLE_CALLS)
		ftrace_replace_code(0);

	if (command & FTRACE_UPDATE_TRACE_FUNC)
		ftrace_update_ftrace_func(ftrace_trace_function);

	if (command & FTRACE_START_FUNC_RET)
		ftrace_enable_ftrace_graph_caller();
	else if (command & FTRACE_STOP_FUNC_RET)
		ftrace_disable_ftrace_graph_caller();
}