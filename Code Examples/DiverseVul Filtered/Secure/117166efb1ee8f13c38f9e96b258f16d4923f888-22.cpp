static int ext4_prepare_inline_data(handle_t *handle, struct inode *inode,
				    unsigned int len)
{
	int ret, size, no_expand;
	struct ext4_inode_info *ei = EXT4_I(inode);

	if (!ext4_test_inode_state(inode, EXT4_STATE_MAY_INLINE_DATA))
		return -ENOSPC;

	size = ext4_get_max_inline_size(inode);
	if (size < len)
		return -ENOSPC;

	ext4_write_lock_xattr(inode, &no_expand);

	if (ei->i_inline_off)
		ret = ext4_update_inline_data(handle, inode, len);
	else
		ret = ext4_create_inline_data(handle, inode, len);

	ext4_write_unlock_xattr(inode, &no_expand);
	return ret;
}