static int register_ftrace_profiler(void)
{
	return register_ftrace_graph(&profile_graph_return,
				     &profile_graph_entry);
}