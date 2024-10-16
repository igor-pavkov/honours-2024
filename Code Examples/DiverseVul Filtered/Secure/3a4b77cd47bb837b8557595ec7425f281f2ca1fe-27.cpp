static int ext4_dummy_context(struct inode *inode)
{
	return DUMMY_ENCRYPTION_ENABLED(EXT4_SB(inode->i_sb));
}