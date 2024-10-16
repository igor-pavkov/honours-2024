static void *fpid_next(struct seq_file *m, void *v, loff_t *pos)
{
	if (v == (void *)1)
		return NULL;

	return seq_list_next(v, &ftrace_pids, pos);
}