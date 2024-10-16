static int show_tid_numa_map(struct seq_file *m, void *v)
{
	return show_numa_map(m, v, 0);
}