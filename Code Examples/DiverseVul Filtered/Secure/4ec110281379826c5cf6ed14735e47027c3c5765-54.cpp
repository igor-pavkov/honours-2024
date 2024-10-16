static int ext4_ui_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ext4_ui_proc_show, PDE(inode)->data);
}