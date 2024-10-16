static int ext4_update_inline_dir(handle_t *handle, struct inode *dir,
				  struct ext4_iloc *iloc)
{
	int ret;
	int old_size = EXT4_I(dir)->i_inline_size - EXT4_MIN_INLINE_DATA_SIZE;
	int new_size = get_max_inline_xattr_value_size(dir, iloc);

	if (new_size - old_size <= EXT4_DIR_REC_LEN(1))
		return -ENOSPC;

	ret = ext4_update_inline_data(handle, dir,
				      new_size + EXT4_MIN_INLINE_DATA_SIZE);
	if (ret)
		return ret;

	ext4_update_final_de(ext4_get_inline_xattr_pos(dir, iloc), old_size,
			     EXT4_I(dir)->i_inline_size -
						EXT4_MIN_INLINE_DATA_SIZE);
	dir->i_size = EXT4_I(dir)->i_disksize = EXT4_I(dir)->i_inline_size;
	return 0;
}