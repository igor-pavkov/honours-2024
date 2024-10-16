static int relatime_need_update(const struct path *path, struct inode *inode,
				struct timespec now, bool rcu)
{

	if (!(path->mnt->mnt_flags & MNT_RELATIME))
		return 1;

	update_ovl_inode_times(path->dentry, inode, rcu);
	/*
	 * Is mtime younger than atime? If yes, update atime:
	 */
	if (timespec64_compare(&inode->i_mtime, &inode->i_atime) >= 0)
		return 1;
	/*
	 * Is ctime younger than atime? If yes, update atime:
	 */
	if (timespec64_compare(&inode->i_ctime, &inode->i_atime) >= 0)
		return 1;

	/*
	 * Is the previous atime value older than a day? If yes,
	 * update atime:
	 */
	if ((long)(now.tv_sec - inode->i_atime.tv_sec) >= 24*60*60)
		return 1;
	/*
	 * Good, we can skip the atime update:
	 */
	return 0;
}