static int selinux_mount(const char *dev_name,
			 const struct path *path,
			 const char *type,
			 unsigned long flags,
			 void *data)
{
	const struct cred *cred = current_cred();

	if (flags & MS_REMOUNT)
		return superblock_has_perm(cred, path->dentry->d_sb,
					   FILESYSTEM__REMOUNT, NULL);
	else
		return path_has_perm(cred, path, FILE__MOUNTON);
}