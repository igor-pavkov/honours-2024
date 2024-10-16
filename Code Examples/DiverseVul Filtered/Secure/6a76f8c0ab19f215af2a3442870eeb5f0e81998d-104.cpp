static void ftrace_shutdown_sysctl(void)
{
	if (unlikely(ftrace_disabled))
		return;

	/* ftrace_start_up is true if ftrace is running */
	if (ftrace_start_up)
		ftrace_run_update_code(FTRACE_DISABLE_CALLS);
}