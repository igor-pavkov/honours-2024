static void set_ftrace_pid_task(struct pid *pid)
{
	if (pid == ftrace_swapper_pid)
		set_ftrace_swapper();
	else
		set_ftrace_pid(pid);
}