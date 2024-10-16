static int ext4_prepare_context(struct inode *inode)
{
	return ext4_convert_inline_data(inode);
}