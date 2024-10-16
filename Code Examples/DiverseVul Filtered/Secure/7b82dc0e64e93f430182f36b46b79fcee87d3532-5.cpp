asmlinkage long sys_statfs(const char __user * path, struct statfs __user * buf)
{
	struct nameidata nd;
	int error;

	error = user_path_walk(path, &nd);
	if (!error) {
		struct statfs tmp;
		error = vfs_statfs_native(nd.dentry, &tmp);
		if (!error && copy_to_user(buf, &tmp, sizeof(tmp)))
			error = -EFAULT;
		path_release(&nd);
	}
	return error;
}