static void clear_ftrace_pid(struct pid *pid)
{
	struct task_struct *p;

	rcu_read_lock();
	do_each_pid_task(pid, PIDTYPE_PID, p) {
		clear_tsk_trace_trace(p);
	} while_each_pid_task(pid, PIDTYPE_PID, p);
	rcu_read_unlock();

	put_pid(pid);
}