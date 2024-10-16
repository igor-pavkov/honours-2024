static void ftrace_startup_sysctl(void)
{
	if (unlikely(ftrace_disabled))
		return;

	/* Force update next time */
	saved_ftrace_func = NULL;
	/* ftrace_start_up is true if we want ftrace running */
	if (ftrace_start_up)
		ftrace_run_update_code(FTRACE_UPDATE_CALLS);
}