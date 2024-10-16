static int dm_release(struct inode *inode, struct file *filp)
{
	kfree(filp->private_data);
	return 0;
}