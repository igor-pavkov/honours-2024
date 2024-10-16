asmlinkage long sys_chdir(const char __user * filename)
{
	struct nameidata nd;
	int error;

	error = __user_walk(filename,
			    LOOKUP_FOLLOW|LOOKUP_DIRECTORY|LOOKUP_CHDIR, &nd);
	if (error)
		goto out;

	error = vfs_permission(&nd, MAY_EXEC);
	if (error)
		goto dput_and_out;

	set_fs_pwd(current->fs, nd.mnt, nd.dentry);

dput_and_out:
	path_release(&nd);
out:
	return error;
}