static int clusterip_seq_show(struct seq_file *s, void *v)
{
	struct clusterip_seq_position *idx = v;

	if (idx->pos != 0)
		seq_putc(s, ',');

	seq_printf(s, "%u", idx->bit);

	if (idx->pos == idx->weight - 1)
		seq_putc(s, '\n');

	return 0;
}