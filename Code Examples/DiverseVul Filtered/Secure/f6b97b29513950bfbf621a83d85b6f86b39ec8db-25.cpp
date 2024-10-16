static int nr_info_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &nr_info_seqops);
}