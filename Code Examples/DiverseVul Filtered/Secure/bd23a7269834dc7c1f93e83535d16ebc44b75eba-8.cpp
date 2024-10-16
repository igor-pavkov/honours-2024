static int vbg_misc_device_close(struct inode *inode, struct file *filp)
{
	vbg_core_close_session(filp->private_data);
	filp->private_data = NULL;
	return 0;
}