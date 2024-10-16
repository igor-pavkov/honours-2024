int ext4_try_add_inline_entry(handle_t *handle, struct ext4_filename *fname,
			      struct inode *dir, struct inode *inode)
{
	int ret, inline_size, no_expand;
	void *inline_start;
	struct ext4_iloc iloc;

	ret = ext4_get_inode_loc(dir, &iloc);
	if (ret)
		return ret;

	ext4_write_lock_xattr(dir, &no_expand);
	if (!ext4_has_inline_data(dir))
		goto out;

	inline_start = (void *)ext4_raw_inode(&iloc)->i_block +
						 EXT4_INLINE_DOTDOT_SIZE;
	inline_size = EXT4_MIN_INLINE_DATA_SIZE - EXT4_INLINE_DOTDOT_SIZE;

	ret = ext4_add_dirent_to_inline(handle, fname, dir, inode, &iloc,
					inline_start, inline_size);
	if (ret != -ENOSPC)
		goto out;

	/* check whether it can be inserted to inline xattr space. */
	inline_size = EXT4_I(dir)->i_inline_size -
			EXT4_MIN_INLINE_DATA_SIZE;
	if (!inline_size) {
		/* Try to use the xattr space.*/
		ret = ext4_update_inline_dir(handle, dir, &iloc);
		if (ret && ret != -ENOSPC)
			goto out;

		inline_size = EXT4_I(dir)->i_inline_size -
				EXT4_MIN_INLINE_DATA_SIZE;
	}

	if (inline_size) {
		inline_start = ext4_get_inline_xattr_pos(dir, &iloc);

		ret = ext4_add_dirent_to_inline(handle, fname, dir,
						inode, &iloc, inline_start,
						inline_size);

		if (ret != -ENOSPC)
			goto out;
	}

	/*
	 * The inline space is filled up, so create a new block for it.
	 * As the extent tree will be created, we have to save the inline
	 * dir first.
	 */
	ret = ext4_convert_inline_data_nolock(handle, dir, &iloc);

out:
	ext4_write_unlock_xattr(dir, &no_expand);
	ext4_mark_inode_dirty(handle, dir);
	brelse(iloc.bh);
	return ret;
}