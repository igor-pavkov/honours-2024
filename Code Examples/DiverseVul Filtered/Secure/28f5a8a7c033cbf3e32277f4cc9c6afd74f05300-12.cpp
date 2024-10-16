static int ocfs2_dir_open(struct inode *inode, struct file *file)
{
	return ocfs2_init_file_private(inode, file);
}