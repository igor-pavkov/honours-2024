int vfs_open(const struct path *path, struct file *file,
	     const struct cred *cred)
{
	struct inode *inode = vfs_select_inode(path->dentry, file->f_flags);

	if (IS_ERR(inode))
		return PTR_ERR(inode);

	file->f_path = *path;
	return do_dentry_open(file, inode, NULL, cred);
}