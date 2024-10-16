static void *nr_info_next(struct seq_file *seq, void *v, loff_t *pos)
{
	++*pos;

	return (v == SEQ_START_TOKEN) ? sk_head(&nr_list)
		: sk_next((struct sock *)v);
}