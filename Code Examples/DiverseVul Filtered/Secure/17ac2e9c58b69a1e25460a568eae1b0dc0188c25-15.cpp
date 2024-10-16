static void *rose_info_next(struct seq_file *seq, void *v, loff_t *pos)
{
	++*pos;

	return (v == SEQ_START_TOKEN) ? sk_head(&rose_list)
		: sk_next((struct sock *)v);
}