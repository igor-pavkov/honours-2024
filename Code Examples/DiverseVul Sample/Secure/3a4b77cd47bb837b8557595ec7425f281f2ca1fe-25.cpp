static void lockdep_set_quota_inode(struct inode *inode, int subclass)
{
	struct ext4_inode_info *ei = EXT4_I(inode);

	/* The first argument of lockdep_set_subclass has to be
	 * *exactly* the same as the argument to init_rwsem() --- in
	 * this case, in init_once() --- or lockdep gets unhappy
	 * because the name of the lock is set using the
	 * stringification of the argument to init_rwsem().
	 */
	(void) ei;	/* shut up clang warning if !CONFIG_LOCKDEP */
	lockdep_set_subclass(&ei->i_data_sem, subclass);
}