int vfs_open(const struct path *path, struct file *file,
	     const struct cred *cred)
{
	struct dentry *dentry = path->dentry;
	struct inode *inode = dentry->d_inode;

	file->f_path = *path;
	if (dentry->d_flags & DCACHE_OP_SELECT_INODE) {
		inode = dentry->d_op->d_select_inode(dentry, file->f_flags);
		if (IS_ERR(inode))
			return PTR_ERR(inode);
	}

	return do_dentry_open(file, inode, NULL, cred);
}