static void set_ftrace_pid(struct pid *pid)
{
	struct task_struct *p;

	rcu_read_lock();
	do_each_pid_task(pid, PIDTYPE_PID, p) {
		set_tsk_trace_trace(p);
	} while_each_pid_task(pid, PIDTYPE_PID, p);
	rcu_read_unlock();
}