static int sco_debugfs_open(struct inode *inode, struct file *file)
{
	return single_open(file, sco_debugfs_show, inode->i_private);
}