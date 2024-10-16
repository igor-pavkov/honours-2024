asmlinkage long sys_statfs64(const char __user *path, size_t sz, struct statfs64 __user *buf)
{
	struct nameidata nd;
	long error;

	if (sz != sizeof(*buf))
		return -EINVAL;
	error = user_path_walk(path, &nd);
	if (!error) {
		struct statfs64 tmp;
		error = vfs_statfs64(nd.dentry, &tmp);
		if (!error && copy_to_user(buf, &tmp, sizeof(tmp)))
			error = -EFAULT;
		path_release(&nd);
	}
	return error;
}