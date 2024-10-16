static void ftrace_update_pid_func(void)
{
	/* Only do something if we are tracing something */
	if (ftrace_trace_function == ftrace_stub)
		return;

	update_ftrace_function();
}