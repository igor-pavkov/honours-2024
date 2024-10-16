struct buffer_head *ext4_find_inline_entry(struct inode *dir,
					struct ext4_filename *fname,
					struct ext4_dir_entry_2 **res_dir,
					int *has_inline_data)
{
	int ret;
	struct ext4_iloc iloc;
	void *inline_start;
	int inline_size;

	if (ext4_get_inode_loc(dir, &iloc))
		return NULL;

	down_read(&EXT4_I(dir)->xattr_sem);
	if (!ext4_has_inline_data(dir)) {
		*has_inline_data = 0;
		goto out;
	}

	inline_start = (void *)ext4_raw_inode(&iloc)->i_block +
						EXT4_INLINE_DOTDOT_SIZE;
	inline_size = EXT4_MIN_INLINE_DATA_SIZE - EXT4_INLINE_DOTDOT_SIZE;
	ret = ext4_search_dir(iloc.bh, inline_start, inline_size,
			      dir, fname, 0, res_dir);
	if (ret == 1)
		goto out_find;
	if (ret < 0)
		goto out;

	if (ext4_get_inline_size(dir) == EXT4_MIN_INLINE_DATA_SIZE)
		goto out;

	inline_start = ext4_get_inline_xattr_pos(dir, &iloc);
	inline_size = ext4_get_inline_size(dir) - EXT4_MIN_INLINE_DATA_SIZE;

	ret = ext4_search_dir(iloc.bh, inline_start, inline_size,
			      dir, fname, 0, res_dir);
	if (ret == 1)
		goto out_find;

out:
	brelse(iloc.bh);
	iloc.bh = NULL;
out_find:
	up_read(&EXT4_I(dir)->xattr_sem);
	return iloc.bh;
}