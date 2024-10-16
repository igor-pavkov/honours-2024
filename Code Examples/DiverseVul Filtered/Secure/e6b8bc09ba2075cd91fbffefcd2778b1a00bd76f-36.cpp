static void ext4_update_dx_flag(struct inode *inode)
{
	if (!EXT4_HAS_COMPAT_FEATURE(inode->i_sb,
				     EXT4_FEATURE_COMPAT_DIR_INDEX))
		EXT4_I(inode)->i_flags &= ~EXT4_INDEX_FL;
}