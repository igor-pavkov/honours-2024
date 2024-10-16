void clear_ftrace_function(void)
{
	ftrace_trace_function = ftrace_stub;
	ftrace_pid_function = ftrace_stub;
}