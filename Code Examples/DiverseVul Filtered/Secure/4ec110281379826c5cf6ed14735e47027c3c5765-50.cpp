static void ext4_clear_inode(struct inode *inode)
{
#ifdef CONFIG_EXT4_FS_POSIX_ACL
	if (EXT4_I(inode)->i_acl &&
			EXT4_I(inode)->i_acl != EXT4_ACL_NOT_CACHED) {
		posix_acl_release(EXT4_I(inode)->i_acl);
		EXT4_I(inode)->i_acl = EXT4_ACL_NOT_CACHED;
	}
	if (EXT4_I(inode)->i_default_acl &&
			EXT4_I(inode)->i_default_acl != EXT4_ACL_NOT_CACHED) {
		posix_acl_release(EXT4_I(inode)->i_default_acl);
		EXT4_I(inode)->i_default_acl = EXT4_ACL_NOT_CACHED;
	}
#endif
	ext4_discard_preallocations(inode);
	if (EXT4_JOURNAL(inode))
		jbd2_journal_release_jbd_inode(EXT4_SB(inode->i_sb)->s_journal,
				       &EXT4_I(inode)->jinode);
}