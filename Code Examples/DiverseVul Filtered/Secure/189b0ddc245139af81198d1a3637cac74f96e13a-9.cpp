static int proc_dopipe_max_size(struct ctl_table *table, int write,
				void *buffer, size_t *lenp, loff_t *ppos)
{
	return do_proc_douintvec(table, write, buffer, lenp, ppos,
				 do_proc_dopipe_max_size_conv, NULL);
}