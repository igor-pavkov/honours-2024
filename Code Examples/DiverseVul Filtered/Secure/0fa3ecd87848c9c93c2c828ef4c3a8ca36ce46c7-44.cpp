static void update_ovl_inode_times(struct dentry *dentry, struct inode *inode,
			       bool rcu)
{
	struct dentry *upperdentry;

	/*
	 * Nothing to do if in rcu or if non-overlayfs
	 */
	if (rcu || likely(!(dentry->d_flags & DCACHE_OP_REAL)))
		return;

	upperdentry = d_real(dentry, NULL, 0, D_REAL_UPPER);

	/*
	 * If file is on lower then we can't update atime, so no worries about
	 * stale mtime/ctime.
	 */
	if (upperdentry) {
		struct inode *realinode = d_inode(upperdentry);

		if ((!timespec64_equal(&inode->i_mtime, &realinode->i_mtime) ||
		     !timespec64_equal(&inode->i_ctime, &realinode->i_ctime))) {
			inode->i_mtime = realinode->i_mtime;
			inode->i_ctime = realinode->i_ctime;
		}
	}
}