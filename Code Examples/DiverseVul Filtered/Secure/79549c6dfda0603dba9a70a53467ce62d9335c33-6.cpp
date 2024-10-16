const struct cred *get_task_cred(struct task_struct *task)
{
	const struct cred *cred;

	rcu_read_lock();

	do {
		cred = __task_cred((task));
		BUG_ON(!cred);
	} while (!atomic_inc_not_zero(&((struct cred *)cred)->usage));

	rcu_read_unlock();
	return cred;
}