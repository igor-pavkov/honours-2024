static int ext4_create_inline_data(handle_t *handle,
				   struct inode *inode, unsigned len)
{
	int error;
	void *value = NULL;
	struct ext4_xattr_ibody_find is = {
		.s = { .not_found = -ENODATA, },
	};
	struct ext4_xattr_info i = {
		.name_index = EXT4_XATTR_INDEX_SYSTEM,
		.name = EXT4_XATTR_SYSTEM_DATA,
	};

	error = ext4_get_inode_loc(inode, &is.iloc);
	if (error)
		return error;

	BUFFER_TRACE(is.iloc.bh, "get_write_access");
	error = ext4_journal_get_write_access(handle, is.iloc.bh);
	if (error)
		goto out;

	if (len > EXT4_MIN_INLINE_DATA_SIZE) {
		value = EXT4_ZERO_XATTR_VALUE;
		len -= EXT4_MIN_INLINE_DATA_SIZE;
	} else {
		value = "";
		len = 0;
	}

	/* Insert the the xttr entry. */
	i.value = value;
	i.value_len = len;

	error = ext4_xattr_ibody_find(inode, &i, &is);
	if (error)
		goto out;

	BUG_ON(!is.s.not_found);

	error = ext4_xattr_ibody_inline_set(handle, inode, &i, &is);
	if (error) {
		if (error == -ENOSPC)
			ext4_clear_inode_state(inode,
					       EXT4_STATE_MAY_INLINE_DATA);
		goto out;
	}

	memset((void *)ext4_raw_inode(&is.iloc)->i_block,
		0, EXT4_MIN_INLINE_DATA_SIZE);

	EXT4_I(inode)->i_inline_off = (u16)((void *)is.s.here -
				      (void *)ext4_raw_inode(&is.iloc));
	EXT4_I(inode)->i_inline_size = len + EXT4_MIN_INLINE_DATA_SIZE;
	ext4_clear_inode_flag(inode, EXT4_INODE_EXTENTS);
	ext4_set_inode_flag(inode, EXT4_INODE_INLINE_DATA);
	get_bh(is.iloc.bh);
	error = ext4_mark_iloc_dirty(handle, inode, &is.iloc);

out:
	brelse(is.iloc.bh);
	return error;
}