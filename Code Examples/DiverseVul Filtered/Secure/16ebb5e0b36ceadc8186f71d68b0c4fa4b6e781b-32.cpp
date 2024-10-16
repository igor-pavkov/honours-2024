static int psched_open(struct inode *inode, struct file *file)
{
	return single_open(file, psched_show, PDE(inode)->data);
}