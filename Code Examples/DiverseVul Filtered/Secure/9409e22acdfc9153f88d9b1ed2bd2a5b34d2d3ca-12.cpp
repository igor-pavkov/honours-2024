static bool path_connected(const struct path *path)
{
	struct vfsmount *mnt = path->mnt;

	/* Only bind mounts can have disconnected paths */
	if (mnt->mnt_root == mnt->mnt_sb->s_root)
		return true;

	return is_subdir(path->dentry, mnt->mnt_root);
}