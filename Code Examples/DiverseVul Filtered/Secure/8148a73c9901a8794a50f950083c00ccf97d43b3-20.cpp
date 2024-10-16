static int proc_setgroups_open(struct inode *inode, struct file *file)
{
	struct user_namespace *ns = NULL;
	struct task_struct *task;
	int ret;

	ret = -ESRCH;
	task = get_proc_task(inode);
	if (task) {
		rcu_read_lock();
		ns = get_user_ns(task_cred_xxx(task, user_ns));
		rcu_read_unlock();
		put_task_struct(task);
	}
	if (!ns)
		goto err;

	if (file->f_mode & FMODE_WRITE) {
		ret = -EACCES;
		if (!ns_capable(ns, CAP_SYS_ADMIN))
			goto err_put_ns;
	}

	ret = single_open(file, &proc_setgroups_show, ns);
	if (ret)
		goto err_put_ns;

	return 0;
err_put_ns:
	put_user_ns(ns);
err:
	return ret;
}