asmlinkage long sys_umount(char __user * name, int flags)
{
	struct nameidata nd;
	int retval;

	retval = __user_walk(name, LOOKUP_FOLLOW, &nd);
	if (retval)
		goto out;
	retval = -EINVAL;
	if (nd.dentry != nd.mnt->mnt_root)
		goto dput_and_out;
	if (!check_mnt(nd.mnt))
		goto dput_and_out;

	retval = -EPERM;
	if (!capable(CAP_SYS_ADMIN))
		goto dput_and_out;

	retval = do_umount(nd.mnt, flags);
dput_and_out:
	path_release_on_umount(&nd);
out:
	return retval;
}