static int btrfs_getattr(struct vfsmount *mnt,
			 struct dentry *dentry, struct kstat *stat)
{
	u64 delalloc_bytes;
	struct inode *inode = d_inode(dentry);
	u32 blocksize = inode->i_sb->s_blocksize;

	generic_fillattr(inode, stat);
	stat->dev = BTRFS_I(inode)->root->anon_dev;
	stat->blksize = PAGE_CACHE_SIZE;

	spin_lock(&BTRFS_I(inode)->lock);
	delalloc_bytes = BTRFS_I(inode)->delalloc_bytes;
	spin_unlock(&BTRFS_I(inode)->lock);
	stat->blocks = (ALIGN(inode_get_bytes(inode), blocksize) +
			ALIGN(delalloc_bytes, blocksize)) >> 9;
	return 0;
}