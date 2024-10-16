int ext4_inline_data_fiemap(struct inode *inode,
			    struct fiemap_extent_info *fieinfo,
			    int *has_inline, __u64 start, __u64 len)
{
	__u64 physical = 0;
	__u64 inline_len;
	__u32 flags = FIEMAP_EXTENT_DATA_INLINE | FIEMAP_EXTENT_NOT_ALIGNED |
		FIEMAP_EXTENT_LAST;
	int error = 0;
	struct ext4_iloc iloc;

	down_read(&EXT4_I(inode)->xattr_sem);
	if (!ext4_has_inline_data(inode)) {
		*has_inline = 0;
		goto out;
	}
	inline_len = min_t(size_t, ext4_get_inline_size(inode),
			   i_size_read(inode));
	if (start >= inline_len)
		goto out;
	if (start + len < inline_len)
		inline_len = start + len;
	inline_len -= start;

	error = ext4_get_inode_loc(inode, &iloc);
	if (error)
		goto out;

	physical = (__u64)iloc.bh->b_blocknr << inode->i_sb->s_blocksize_bits;
	physical += (char *)ext4_raw_inode(&iloc) - iloc.bh->b_data;
	physical += offsetof(struct ext4_inode, i_block);

	if (physical)
		error = fiemap_fill_next_extent(fieinfo, start, physical,
						inline_len, flags);
	brelse(iloc.bh);
out:
	up_read(&EXT4_I(inode)->xattr_sem);
	return (error < 0 ? error : 0);
}