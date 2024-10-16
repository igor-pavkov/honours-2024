static int proc_pid_limits(struct seq_file *m, struct pid_namespace *ns,
			   struct pid *pid, struct task_struct *task)
{
	unsigned int i;
	unsigned long flags;

	struct rlimit rlim[RLIM_NLIMITS];

	if (!lock_task_sighand(task, &flags))
		return 0;
	memcpy(rlim, task->signal->rlim, sizeof(struct rlimit) * RLIM_NLIMITS);
	unlock_task_sighand(task, &flags);

	/*
	 * print the file header
	 */
       seq_printf(m, "%-25s %-20s %-20s %-10s\n",
		  "Limit", "Soft Limit", "Hard Limit", "Units");

	for (i = 0; i < RLIM_NLIMITS; i++) {
		if (rlim[i].rlim_cur == RLIM_INFINITY)
			seq_printf(m, "%-25s %-20s ",
				   lnames[i].name, "unlimited");
		else
			seq_printf(m, "%-25s %-20lu ",
				   lnames[i].name, rlim[i].rlim_cur);

		if (rlim[i].rlim_max == RLIM_INFINITY)
			seq_printf(m, "%-20s ", "unlimited");
		else
			seq_printf(m, "%-20lu ", rlim[i].rlim_max);

		if (lnames[i].unit)
			seq_printf(m, "%-10s\n", lnames[i].unit);
		else
			seq_putc(m, '\n');
	}

	return 0;
}