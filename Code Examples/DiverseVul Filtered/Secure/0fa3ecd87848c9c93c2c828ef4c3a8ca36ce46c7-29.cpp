static int no_open(struct inode *inode, struct file *file)
{
	return -ENXIO;
}