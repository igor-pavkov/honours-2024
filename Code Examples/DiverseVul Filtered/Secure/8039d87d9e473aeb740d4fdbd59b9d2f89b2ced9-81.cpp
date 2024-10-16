void btrfs_update_iflags(struct inode *inode)
{
	struct btrfs_inode *ip = BTRFS_I(inode);
	unsigned int new_fl = 0;

	if (ip->flags & BTRFS_INODE_SYNC)
		new_fl |= S_SYNC;
	if (ip->flags & BTRFS_INODE_IMMUTABLE)
		new_fl |= S_IMMUTABLE;
	if (ip->flags & BTRFS_INODE_APPEND)
		new_fl |= S_APPEND;
	if (ip->flags & BTRFS_INODE_NOATIME)
		new_fl |= S_NOATIME;
	if (ip->flags & BTRFS_INODE_DIRSYNC)
		new_fl |= S_DIRSYNC;

	set_mask_bits(&inode->i_flags,
		      S_SYNC | S_APPEND | S_IMMUTABLE | S_NOATIME | S_DIRSYNC,
		      new_fl);
}