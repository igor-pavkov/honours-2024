static void *proto_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	return seq_list_next(v, &proto_list, pos);
}