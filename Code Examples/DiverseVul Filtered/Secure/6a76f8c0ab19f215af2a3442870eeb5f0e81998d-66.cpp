void ftrace_kill(void)
{
	ftrace_disabled = 1;
	ftrace_enabled = 0;
	clear_ftrace_function();
}