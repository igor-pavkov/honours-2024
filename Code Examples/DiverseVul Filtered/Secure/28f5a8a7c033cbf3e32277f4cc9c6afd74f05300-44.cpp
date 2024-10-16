static int ocfs2_is_io_unaligned(struct inode *inode, size_t count, loff_t pos)
{
	int blockmask = inode->i_sb->s_blocksize - 1;
	loff_t final_size = pos + count;

	if ((pos & blockmask) || (final_size & blockmask))
		return 1;
	return 0;
}