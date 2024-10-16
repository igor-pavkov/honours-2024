asmlinkage long sys_chown(const char __user * filename, uid_t user, gid_t group)
{
	struct nameidata nd;
	int error;

	error = user_path_walk(filename, &nd);
	if (error)
		goto out;
	error = chown_common(nd.dentry, user, group);
	path_release(&nd);
out:
	return error;
}