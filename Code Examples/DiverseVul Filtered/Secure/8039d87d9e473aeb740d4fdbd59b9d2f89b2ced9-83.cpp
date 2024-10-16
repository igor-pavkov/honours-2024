static int extent_same_check_offsets(struct inode *inode, u64 off, u64 *plen,
				     u64 olen)
{
	u64 len = *plen;
	u64 bs = BTRFS_I(inode)->root->fs_info->sb->s_blocksize;

	if (off + olen > inode->i_size || off + olen < off)
		return -EINVAL;

	/* if we extend to eof, continue to block boundary */
	if (off + len == inode->i_size)
		*plen = len = ALIGN(inode->i_size, bs) - off;

	/* Check that we are block aligned - btrfs_clone() requires this */
	if (!IS_ALIGNED(off, bs) || !IS_ALIGNED(off + len, bs))
		return -EINVAL;

	return 0;
}