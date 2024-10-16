asmlinkage long sys_fstatfs64(unsigned int fd, size_t sz, struct statfs64 __user *buf)
{
	struct file * file;
	struct statfs64 tmp;
	int error;

	if (sz != sizeof(*buf))
		return -EINVAL;

	error = -EBADF;
	file = fget(fd);
	if (!file)
		goto out;
	error = vfs_statfs64(file->f_path.dentry, &tmp);
	if (!error && copy_to_user(buf, &tmp, sizeof(tmp)))
		error = -EFAULT;
	fput(file);
out:
	return error;
}