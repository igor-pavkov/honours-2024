int generic_update_time(struct inode *inode, struct timespec64 *time, int flags)
{
	int iflags = I_DIRTY_TIME;
	bool dirty = false;

	if (flags & S_ATIME)
		inode->i_atime = *time;
	if (flags & S_VERSION)
		dirty = inode_maybe_inc_iversion(inode, false);
	if (flags & S_CTIME)
		inode->i_ctime = *time;
	if (flags & S_MTIME)
		inode->i_mtime = *time;
	if ((flags & (S_ATIME | S_CTIME | S_MTIME)) &&
	    !(inode->i_sb->s_flags & SB_LAZYTIME))
		dirty = true;

	if (dirty)
		iflags |= I_DIRTY_SYNC;
	__mark_inode_dirty(inode, iflags);
	return 0;
}