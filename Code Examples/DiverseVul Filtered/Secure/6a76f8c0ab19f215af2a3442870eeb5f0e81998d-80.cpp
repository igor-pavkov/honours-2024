static int ftrace_pid_add(int p)
{
	struct pid *pid;
	struct ftrace_pid *fpid;
	int ret = -EINVAL;

	mutex_lock(&ftrace_lock);

	if (!p)
		pid = ftrace_swapper_pid;
	else
		pid = find_get_pid(p);

	if (!pid)
		goto out;

	ret = 0;

	list_for_each_entry(fpid, &ftrace_pids, list)
		if (fpid->pid == pid)
			goto out_put;

	ret = -ENOMEM;

	fpid = kmalloc(sizeof(*fpid), GFP_KERNEL);
	if (!fpid)
		goto out_put;

	list_add(&fpid->list, &ftrace_pids);
	fpid->pid = pid;

	set_ftrace_pid_task(pid);

	ftrace_update_pid_func();
	ftrace_startup_enable(0);

	mutex_unlock(&ftrace_lock);
	return 0;

out_put:
	if (pid != ftrace_swapper_pid)
		put_pid(pid);

out:
	mutex_unlock(&ftrace_lock);
	return ret;
}