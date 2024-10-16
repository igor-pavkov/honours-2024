static void __init set_ftrace_early_filters(void)
{
	if (ftrace_filter_buf[0])
		ftrace_set_early_filter(&global_ops, ftrace_filter_buf, 1);
	if (ftrace_notrace_buf[0])
		ftrace_set_early_filter(&global_ops, ftrace_notrace_buf, 0);
#ifdef CONFIG_FUNCTION_GRAPH_TRACER
	if (ftrace_graph_buf[0])
		set_ftrace_early_graph(ftrace_graph_buf);
#endif /* CONFIG_FUNCTION_GRAPH_TRACER */
}