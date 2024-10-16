static pid_t pid_of_stack(struct proc_maps_private *priv,
				struct vm_area_struct *vma, bool is_pid)
{
	struct inode *inode = priv->inode;
	struct task_struct *task;
	pid_t ret = 0;

	rcu_read_lock();
	task = pid_task(proc_pid(inode), PIDTYPE_PID);
	if (task) {
		task = task_of_stack(task, vma, is_pid);
		if (task)
			ret = task_pid_nr_ns(task, inode->i_sb->s_fs_info);
	}
	rcu_read_unlock();

	return ret;
}