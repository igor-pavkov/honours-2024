static int do_open(struct inode *inode, struct file *filp)
{
	nfs_fscache_open_file(inode, filp);
	return 0;
}