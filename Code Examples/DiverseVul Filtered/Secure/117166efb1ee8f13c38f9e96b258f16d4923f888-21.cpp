int ext4_get_max_inline_size(struct inode *inode)
{
	int error, max_inline_size;
	struct ext4_iloc iloc;

	if (EXT4_I(inode)->i_extra_isize == 0)
		return 0;

	error = ext4_get_inode_loc(inode, &iloc);
	if (error) {
		ext4_error_inode(inode, __func__, __LINE__, 0,
				 "can't get inode location %lu",
				 inode->i_ino);
		return 0;
	}

	down_read(&EXT4_I(inode)->xattr_sem);
	max_inline_size = get_max_inline_xattr_value_size(inode, &iloc);
	up_read(&EXT4_I(inode)->xattr_sem);

	brelse(iloc.bh);

	if (!max_inline_size)
		return 0;

	return max_inline_size + EXT4_MIN_INLINE_DATA_SIZE;
}