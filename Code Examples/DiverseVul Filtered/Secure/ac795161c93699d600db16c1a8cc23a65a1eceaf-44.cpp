static int nfs_finish_open(struct nfs_open_context *ctx,
			   struct dentry *dentry,
			   struct file *file, unsigned open_flags)
{
	int err;

	err = finish_open(file, dentry, do_open);
	if (err)
		goto out;
	if (S_ISREG(file->f_path.dentry->d_inode->i_mode))
		nfs_file_set_open_context(file, ctx);
	else
		err = -EOPENSTALE;
out:
	return err;
}