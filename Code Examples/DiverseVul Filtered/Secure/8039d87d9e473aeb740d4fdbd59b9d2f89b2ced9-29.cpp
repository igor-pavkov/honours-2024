void btrfs_inherit_iflags(struct inode *inode, struct inode *dir)
{
	unsigned int flags;

	if (!dir)
		return;

	flags = BTRFS_I(dir)->flags;

	if (flags & BTRFS_INODE_NOCOMPRESS) {
		BTRFS_I(inode)->flags &= ~BTRFS_INODE_COMPRESS;
		BTRFS_I(inode)->flags |= BTRFS_INODE_NOCOMPRESS;
	} else if (flags & BTRFS_INODE_COMPRESS) {
		BTRFS_I(inode)->flags &= ~BTRFS_INODE_NOCOMPRESS;
		BTRFS_I(inode)->flags |= BTRFS_INODE_COMPRESS;
	}

	if (flags & BTRFS_INODE_NODATACOW) {
		BTRFS_I(inode)->flags |= BTRFS_INODE_NODATACOW;
		if (S_ISREG(inode->i_mode))
			BTRFS_I(inode)->flags |= BTRFS_INODE_NODATASUM;
	}

	btrfs_update_iflags(inode);
}