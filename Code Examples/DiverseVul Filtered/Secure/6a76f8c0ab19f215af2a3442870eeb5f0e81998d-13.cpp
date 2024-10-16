static int profile_graph_entry(struct ftrace_graph_ent *trace)
{
	function_profile_call(trace->func, 0, NULL, NULL);
	return 1;
}