int ext4_destroy_inline_data(handle_t *handle, struct inode *inode)
{
	int ret, no_expand;

	ext4_write_lock_xattr(inode, &no_expand);
	ret = ext4_destroy_inline_data_nolock(handle, inode);
	ext4_write_unlock_xattr(inode, &no_expand);

	return ret;
}