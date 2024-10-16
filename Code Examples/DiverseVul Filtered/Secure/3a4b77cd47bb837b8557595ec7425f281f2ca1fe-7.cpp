static unsigned ext4_max_namelen(struct inode *inode)
{
	return S_ISLNK(inode->i_mode) ? inode->i_sb->s_blocksize :
		EXT4_NAME_LEN;
}