static void ext4_dec_count(handle_t *handle, struct inode *inode)
{
	drop_nlink(inode);
	if (S_ISDIR(inode->i_mode) && inode->i_nlink == 0)
		inc_nlink(inode);
}