static int partitions_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &partitions_op);
}