static __init int ftrace_init_debugfs(void)
{
	struct dentry *d_tracer;

	d_tracer = tracing_init_dentry();
	if (!d_tracer)
		return 0;

	ftrace_init_dyn_debugfs(d_tracer);

	trace_create_file("set_ftrace_pid", 0644, d_tracer,
			    NULL, &ftrace_pid_fops);

	ftrace_profile_debugfs(d_tracer);

	return 0;
}