int ext4_try_create_inline_dir(handle_t *handle, struct inode *parent,
			       struct inode *inode)
{
	int ret, inline_size = EXT4_MIN_INLINE_DATA_SIZE;
	struct ext4_iloc iloc;
	struct ext4_dir_entry_2 *de;

	ret = ext4_get_inode_loc(inode, &iloc);
	if (ret)
		return ret;

	ret = ext4_prepare_inline_data(handle, inode, inline_size);
	if (ret)
		goto out;

	/*
	 * For inline dir, we only save the inode information for the ".."
	 * and create a fake dentry to cover the left space.
	 */
	de = (struct ext4_dir_entry_2 *)ext4_raw_inode(&iloc)->i_block;
	de->inode = cpu_to_le32(parent->i_ino);
	de = (struct ext4_dir_entry_2 *)((void *)de + EXT4_INLINE_DOTDOT_SIZE);
	de->inode = 0;
	de->rec_len = ext4_rec_len_to_disk(
				inline_size - EXT4_INLINE_DOTDOT_SIZE,
				inline_size);
	set_nlink(inode, 2);
	inode->i_size = EXT4_I(inode)->i_disksize = inline_size;
out:
	brelse(iloc.bh);
	return ret;
}