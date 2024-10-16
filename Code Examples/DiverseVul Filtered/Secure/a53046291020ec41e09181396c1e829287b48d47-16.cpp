int jfs_write_inode(struct inode *inode, struct writeback_control *wbc)
{
	int wait = wbc->sync_mode == WB_SYNC_ALL;

	if (inode->i_nlink == 0)
		return 0;
	/*
	 * If COMMIT_DIRTY is not set, the inode isn't really dirty.
	 * It has been committed since the last change, but was still
	 * on the dirty inode list.
	 */
	if (!test_cflag(COMMIT_Dirty, inode)) {
		/* Make sure committed changes hit the disk */
		jfs_flush_journal(JFS_SBI(inode->i_sb)->log, wait);
		return 0;
	}

	if (jfs_commit_inode(inode, wait)) {
		jfs_err("jfs_write_inode: jfs_commit_inode failed!");
		return -EIO;
	} else
		return 0;
}