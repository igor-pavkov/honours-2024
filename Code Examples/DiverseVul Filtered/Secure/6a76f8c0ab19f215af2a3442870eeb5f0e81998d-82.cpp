static __init int ftrace_init_dyn_debugfs(struct dentry *d_tracer)
{

	trace_create_file("available_filter_functions", 0444,
			d_tracer, NULL, &ftrace_avail_fops);

	trace_create_file("enabled_functions", 0444,
			d_tracer, NULL, &ftrace_enabled_fops);

	trace_create_file("set_ftrace_filter", 0644, d_tracer,
			NULL, &ftrace_filter_fops);

	trace_create_file("set_ftrace_notrace", 0644, d_tracer,
				    NULL, &ftrace_notrace_fops);

#ifdef CONFIG_FUNCTION_GRAPH_TRACER
	trace_create_file("set_graph_function", 0444, d_tracer,
				    NULL,
				    &ftrace_graph_fops);
#endif /* CONFIG_FUNCTION_GRAPH_TRACER */

	return 0;
}