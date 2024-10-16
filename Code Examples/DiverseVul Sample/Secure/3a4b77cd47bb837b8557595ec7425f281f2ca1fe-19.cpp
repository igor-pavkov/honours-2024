static int ext4_key_prefix(struct inode *inode, u8 **key)
{
	*key = EXT4_SB(inode->i_sb)->key_prefix;
	return EXT4_SB(inode->i_sb)->key_prefix_size;
}