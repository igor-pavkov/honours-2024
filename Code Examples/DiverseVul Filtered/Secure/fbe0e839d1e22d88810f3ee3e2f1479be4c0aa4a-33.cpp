static struct task_struct *futex_find_get_task(pid_t pid)
{
	struct task_struct *p;

	rcu_read_lock();
	p = find_task_by_vpid(pid);
	if (p)
		get_task_struct(p);

	rcu_read_unlock();

	return p;
}