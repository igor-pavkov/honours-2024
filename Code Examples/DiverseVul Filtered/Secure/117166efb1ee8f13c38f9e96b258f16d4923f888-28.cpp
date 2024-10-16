static int ext4_get_inline_size(struct inode *inode)
{
	if (EXT4_I(inode)->i_inline_off)
		return EXT4_I(inode)->i_inline_size;

	return 0;
}