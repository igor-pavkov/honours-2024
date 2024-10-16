static int fpid_show(struct seq_file *m, void *v)
{
	const struct ftrace_pid *fpid = list_entry(v, struct ftrace_pid, list);

	if (v == (void *)1) {
		seq_printf(m, "no pid\n");
		return 0;
	}

	if (fpid->pid == ftrace_swapper_pid)
		seq_printf(m, "swapper tasks\n");
	else
		seq_printf(m, "%u\n", pid_vnr(fpid->pid));

	return 0;
}