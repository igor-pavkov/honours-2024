static int do_io_accounting(struct task_struct *task, struct seq_file *m, int whole)
{
	struct task_io_accounting acct = task->ioac;
	unsigned long flags;
	int result;

	result = mutex_lock_killable(&task->signal->cred_guard_mutex);
	if (result)
		return result;

	if (!ptrace_may_access(task, PTRACE_MODE_READ_FSCREDS)) {
		result = -EACCES;
		goto out_unlock;
	}

	if (whole && lock_task_sighand(task, &flags)) {
		struct task_struct *t = task;

		task_io_accounting_add(&acct, &task->signal->ioac);
		while_each_thread(task, t)
			task_io_accounting_add(&acct, &t->ioac);

		unlock_task_sighand(task, &flags);
	}
	seq_printf(m,
		   "rchar: %llu\n"
		   "wchar: %llu\n"
		   "syscr: %llu\n"
		   "syscw: %llu\n"
		   "read_bytes: %llu\n"
		   "write_bytes: %llu\n"
		   "cancelled_write_bytes: %llu\n",
		   (unsigned long long)acct.rchar,
		   (unsigned long long)acct.wchar,
		   (unsigned long long)acct.syscr,
		   (unsigned long long)acct.syscw,
		   (unsigned long long)acct.read_bytes,
		   (unsigned long long)acct.write_bytes,
		   (unsigned long long)acct.cancelled_write_bytes);
	result = 0;

out_unlock:
	mutex_unlock(&task->signal->cred_guard_mutex);
	return result;
}