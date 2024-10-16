static int ext4_convert_inline_data_nolock(handle_t *handle,
					   struct inode *inode,
					   struct ext4_iloc *iloc)
{
	int error;
	void *buf = NULL;
	struct buffer_head *data_bh = NULL;
	struct ext4_map_blocks map;
	int inline_size;

	inline_size = ext4_get_inline_size(inode);
	buf = kmalloc(inline_size, GFP_NOFS);
	if (!buf) {
		error = -ENOMEM;
		goto out;
	}

	error = ext4_read_inline_data(inode, buf, inline_size, iloc);
	if (error < 0)
		goto out;

	/*
	 * Make sure the inline directory entries pass checks before we try to
	 * convert them, so that we avoid touching stuff that needs fsck.
	 */
	if (S_ISDIR(inode->i_mode)) {
		error = ext4_check_all_de(inode, iloc->bh,
					buf + EXT4_INLINE_DOTDOT_SIZE,
					inline_size - EXT4_INLINE_DOTDOT_SIZE);
		if (error)
			goto out;
	}

	error = ext4_destroy_inline_data_nolock(handle, inode);
	if (error)
		goto out;

	map.m_lblk = 0;
	map.m_len = 1;
	map.m_flags = 0;
	error = ext4_map_blocks(handle, inode, &map, EXT4_GET_BLOCKS_CREATE);
	if (error < 0)
		goto out_restore;
	if (!(map.m_flags & EXT4_MAP_MAPPED)) {
		error = -EIO;
		goto out_restore;
	}

	data_bh = sb_getblk(inode->i_sb, map.m_pblk);
	if (!data_bh) {
		error = -ENOMEM;
		goto out_restore;
	}

	lock_buffer(data_bh);
	error = ext4_journal_get_create_access(handle, data_bh);
	if (error) {
		unlock_buffer(data_bh);
		error = -EIO;
		goto out_restore;
	}
	memset(data_bh->b_data, 0, inode->i_sb->s_blocksize);

	if (!S_ISDIR(inode->i_mode)) {
		memcpy(data_bh->b_data, buf, inline_size);
		set_buffer_uptodate(data_bh);
		error = ext4_handle_dirty_metadata(handle,
						   inode, data_bh);
	} else {
		error = ext4_finish_convert_inline_dir(handle, inode, data_bh,
						       buf, inline_size);
	}

	unlock_buffer(data_bh);
out_restore:
	if (error)
		ext4_restore_inline_data(handle, inode, iloc, buf, inline_size);

out:
	brelse(data_bh);
	kfree(buf);
	return error;
}