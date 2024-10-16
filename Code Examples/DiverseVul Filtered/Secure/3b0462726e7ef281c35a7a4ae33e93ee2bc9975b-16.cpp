static int cgroup_sane_behavior_show(struct seq_file *seq, void *v)
{
	seq_puts(seq, "0\n");
	return 0;
}