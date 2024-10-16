asmlinkage long sys_fchdir(unsigned int fd)
{
	struct file *file;
	struct dentry *dentry;
	struct inode *inode;
	struct vfsmount *mnt;
	int error;

	error = -EBADF;
	file = fget(fd);
	if (!file)
		goto out;

	dentry = file->f_path.dentry;
	mnt = file->f_path.mnt;
	inode = dentry->d_inode;

	error = -ENOTDIR;
	if (!S_ISDIR(inode->i_mode))
		goto out_putf;

	error = file_permission(file, MAY_EXEC);
	if (!error)
		set_fs_pwd(current->fs, mnt, dentry);
out_putf:
	fput(file);
out:
	return error;
}