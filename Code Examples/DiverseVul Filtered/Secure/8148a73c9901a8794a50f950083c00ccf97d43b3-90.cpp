static int proc_pid_schedstat(struct seq_file *m, struct pid_namespace *ns,
			      struct pid *pid, struct task_struct *task)
{
	if (unlikely(!sched_info_on()))
		seq_printf(m, "0 0 0\n");
	else
		seq_printf(m, "%llu %llu %lu\n",
		   (unsigned long long)task->se.sum_exec_runtime,
		   (unsigned long long)task->sched_info.run_delay,
		   task->sched_info.pcount);

	return 0;
}