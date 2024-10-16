static void *fpid_start(struct seq_file *m, loff_t *pos)
{
	mutex_lock(&ftrace_lock);

	if (list_empty(&ftrace_pids) && (!*pos))
		return (void *) 1;

	return seq_list_start(&ftrace_pids, *pos);
}