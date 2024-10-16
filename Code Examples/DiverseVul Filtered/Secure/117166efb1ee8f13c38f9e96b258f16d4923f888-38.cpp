int ext4_convert_inline_data(struct inode *inode)
{
	int error, needed_blocks, no_expand;
	handle_t *handle;
	struct ext4_iloc iloc;

	if (!ext4_has_inline_data(inode)) {
		ext4_clear_inode_state(inode, EXT4_STATE_MAY_INLINE_DATA);
		return 0;
	}

	needed_blocks = ext4_writepage_trans_blocks(inode);

	iloc.bh = NULL;
	error = ext4_get_inode_loc(inode, &iloc);
	if (error)
		return error;

	handle = ext4_journal_start(inode, EXT4_HT_WRITE_PAGE, needed_blocks);
	if (IS_ERR(handle)) {
		error = PTR_ERR(handle);
		goto out_free;
	}

	ext4_write_lock_xattr(inode, &no_expand);
	if (ext4_has_inline_data(inode))
		error = ext4_convert_inline_data_nolock(handle, inode, &iloc);
	ext4_write_unlock_xattr(inode, &no_expand);
	ext4_journal_stop(handle);
out_free:
	brelse(iloc.bh);
	return error;
}