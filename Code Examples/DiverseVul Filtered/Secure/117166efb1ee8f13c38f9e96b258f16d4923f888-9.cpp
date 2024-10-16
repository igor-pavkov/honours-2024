static void ext4_restore_inline_data(handle_t *handle, struct inode *inode,
				     struct ext4_iloc *iloc,
				     void *buf, int inline_size)
{
	ext4_create_inline_data(handle, inode, inline_size);
	ext4_write_inline_data(inode, iloc, buf, 0, inline_size);
	ext4_set_inode_state(inode, EXT4_STATE_MAY_INLINE_DATA);
}