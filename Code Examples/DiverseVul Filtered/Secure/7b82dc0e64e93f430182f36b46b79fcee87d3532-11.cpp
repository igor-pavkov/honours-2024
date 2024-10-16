asmlinkage long sys_fchown(unsigned int fd, uid_t user, gid_t group)
{
	struct file * file;
	int error = -EBADF;
	struct dentry * dentry;

	file = fget(fd);
	if (!file)
		goto out;

	dentry = file->f_path.dentry;
	audit_inode(NULL, dentry->d_inode);
	error = chown_common(dentry, user, group);
	fput(file);
out:
	return error;
}