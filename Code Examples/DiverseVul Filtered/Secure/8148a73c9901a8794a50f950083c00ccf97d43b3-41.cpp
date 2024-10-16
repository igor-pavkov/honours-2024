struct dentry *proc_pid_lookup(struct inode *dir, struct dentry * dentry, unsigned int flags)
{
	int result = -ENOENT;
	struct task_struct *task;
	unsigned tgid;
	struct pid_namespace *ns;

	tgid = name_to_int(&dentry->d_name);
	if (tgid == ~0U)
		goto out;

	ns = dentry->d_sb->s_fs_info;
	rcu_read_lock();
	task = find_task_by_pid_ns(tgid, ns);
	if (task)
		get_task_struct(task);
	rcu_read_unlock();
	if (!task)
		goto out;

	result = proc_pid_instantiate(dir, dentry, task, NULL);
	put_task_struct(task);
out:
	return ERR_PTR(result);
}