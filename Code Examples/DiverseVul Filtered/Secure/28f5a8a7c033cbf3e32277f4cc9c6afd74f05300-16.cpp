static int ocfs2_init_file_private(struct inode *inode, struct file *file)
{
	struct ocfs2_file_private *fp;

	fp = kzalloc(sizeof(struct ocfs2_file_private), GFP_KERNEL);
	if (!fp)
		return -ENOMEM;

	fp->fp_file = file;
	mutex_init(&fp->fp_mutex);
	ocfs2_file_lock_res_init(&fp->fp_flock, fp);
	file->private_data = fp;

	return 0;
}