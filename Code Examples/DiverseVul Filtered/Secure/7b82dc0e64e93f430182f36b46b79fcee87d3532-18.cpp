asmlinkage long sys_chroot(const char __user * filename)
{
	struct nameidata nd;
	int error;

	error = __user_walk(filename, LOOKUP_FOLLOW | LOOKUP_DIRECTORY | LOOKUP_NOALT, &nd);
	if (error)
		goto out;

	error = vfs_permission(&nd, MAY_EXEC);
	if (error)
		goto dput_and_out;

	error = -EPERM;
	if (!capable(CAP_SYS_CHROOT))
		goto dput_and_out;

	set_fs_root(current->fs, nd.mnt, nd.dentry);
	set_fs_altroot();
	error = 0;
dput_and_out:
	path_release(&nd);
out:
	return error;
}