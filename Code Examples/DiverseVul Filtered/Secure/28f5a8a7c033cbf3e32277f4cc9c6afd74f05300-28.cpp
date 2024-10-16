static void ocfs2_free_file_private(struct inode *inode, struct file *file)
{
	struct ocfs2_file_private *fp = file->private_data;
	struct ocfs2_super *osb = OCFS2_SB(inode->i_sb);

	if (fp) {
		ocfs2_simple_drop_lockres(osb, &fp->fp_flock);
		ocfs2_lock_res_free(&fp->fp_flock);
		kfree(fp);
		file->private_data = NULL;
	}
}