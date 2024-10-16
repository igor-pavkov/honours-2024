asmlinkage long sys_lchown(const char __user * filename, uid_t user, gid_t group)
{
	struct nameidata nd;
	int error;

	error = user_path_walk_link(filename, &nd);
	if (error)
		goto out;
	error = chown_common(nd.dentry, user, group);
	path_release(&nd);
out:
	return error;
}