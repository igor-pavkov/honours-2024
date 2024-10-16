static int register_ftrace_profiler(void)
{
	return register_ftrace_function(&ftrace_profile_ops);
}