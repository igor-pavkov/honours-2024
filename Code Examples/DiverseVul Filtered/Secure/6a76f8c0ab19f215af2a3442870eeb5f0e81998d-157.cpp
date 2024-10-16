static void ftrace_pid_reset(void)
{
	struct ftrace_pid *fpid, *safe;

	mutex_lock(&ftrace_lock);
	list_for_each_entry_safe(fpid, safe, &ftrace_pids, list) {
		struct pid *pid = fpid->pid;

		clear_ftrace_pid_task(pid);

		list_del(&fpid->list);
		kfree(fpid);
	}

	ftrace_update_pid_func();
	ftrace_startup_enable(0);

	mutex_unlock(&ftrace_lock);
}