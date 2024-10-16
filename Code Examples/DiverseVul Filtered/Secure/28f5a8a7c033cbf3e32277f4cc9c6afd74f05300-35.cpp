static int ocfs2_dir_release(struct inode *inode, struct file *file)
{
	ocfs2_free_file_private(inode, file);
	return 0;
}