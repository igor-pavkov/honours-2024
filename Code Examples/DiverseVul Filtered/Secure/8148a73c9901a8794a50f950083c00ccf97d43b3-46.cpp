static int proc_oom_score(struct seq_file *m, struct pid_namespace *ns,
			  struct pid *pid, struct task_struct *task)
{
	unsigned long totalpages = totalram_pages + total_swap_pages;
	unsigned long points = 0;

	read_lock(&tasklist_lock);
	if (pid_alive(task))
		points = oom_badness(task, NULL, NULL, totalpages) *
						1000 / totalpages;
	read_unlock(&tasklist_lock);
	seq_printf(m, "%lu\n", points);

	return 0;
}