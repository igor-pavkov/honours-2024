static u32 ptrace_parent_sid(struct task_struct *task)
{
	u32 sid = 0;
	struct task_struct *tracer;

	rcu_read_lock();
	tracer = ptrace_parent(task);
	if (tracer)
		sid = task_sid(tracer);
	rcu_read_unlock();

	return sid;
}