static void unregister_ftrace_profiler(void)
{
	unregister_ftrace_function(&ftrace_profile_ops);
}