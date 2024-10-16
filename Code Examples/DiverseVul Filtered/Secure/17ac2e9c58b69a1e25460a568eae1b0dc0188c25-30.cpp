static int rose_info_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &rose_info_seqops);
}