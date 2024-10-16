asmlinkage long sys_fchownat(int dfd, const char __user *filename, uid_t user,
			     gid_t group, int flag)
{
	struct nameidata nd;
	int error = -EINVAL;
	int follow;

	if ((flag & ~AT_SYMLINK_NOFOLLOW) != 0)
		goto out;

	follow = (flag & AT_SYMLINK_NOFOLLOW) ? 0 : LOOKUP_FOLLOW;
	error = __user_walk_fd(dfd, filename, follow, &nd);
	if (error)
		goto out;
	error = chown_common(nd.dentry, user, group);
	path_release(&nd);
out:
	return error;
}