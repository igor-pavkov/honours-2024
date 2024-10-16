static int ext4_finish_convert_inline_dir(handle_t *handle,
					  struct inode *inode,
					  struct buffer_head *dir_block,
					  void *buf,
					  int inline_size)
{
	int err, csum_size = 0, header_size = 0;
	struct ext4_dir_entry_2 *de;
	struct ext4_dir_entry_tail *t;
	void *target = dir_block->b_data;

	/*
	 * First create "." and ".." and then copy the dir information
	 * back to the block.
	 */
	de = (struct ext4_dir_entry_2 *)target;
	de = ext4_init_dot_dotdot(inode, de,
		inode->i_sb->s_blocksize, csum_size,
		le32_to_cpu(((struct ext4_dir_entry_2 *)buf)->inode), 1);
	header_size = (void *)de - target;

	memcpy((void *)de, buf + EXT4_INLINE_DOTDOT_SIZE,
		inline_size - EXT4_INLINE_DOTDOT_SIZE);

	if (ext4_has_metadata_csum(inode->i_sb))
		csum_size = sizeof(struct ext4_dir_entry_tail);

	inode->i_size = inode->i_sb->s_blocksize;
	i_size_write(inode, inode->i_sb->s_blocksize);
	EXT4_I(inode)->i_disksize = inode->i_sb->s_blocksize;
	ext4_update_final_de(dir_block->b_data,
			inline_size - EXT4_INLINE_DOTDOT_SIZE + header_size,
			inode->i_sb->s_blocksize - csum_size);

	if (csum_size) {
		t = EXT4_DIRENT_TAIL(dir_block->b_data,
				     inode->i_sb->s_blocksize);
		initialize_dirent_tail(t, inode->i_sb->s_blocksize);
	}
	set_buffer_uptodate(dir_block);
	err = ext4_handle_dirty_dirent_node(handle, inode, dir_block);
	if (err)
		return err;
	set_buffer_verified(dir_block);
	return ext4_mark_inode_dirty(handle, inode);
}