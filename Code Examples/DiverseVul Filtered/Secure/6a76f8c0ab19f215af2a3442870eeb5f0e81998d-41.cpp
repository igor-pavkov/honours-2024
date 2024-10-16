static void clear_ftrace_pid_task(struct pid *pid)
{
	if (pid == ftrace_swapper_pid)
		clear_ftrace_swapper();
	else
		clear_ftrace_pid(pid);
}