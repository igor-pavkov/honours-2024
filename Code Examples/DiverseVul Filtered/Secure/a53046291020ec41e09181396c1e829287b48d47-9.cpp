void jfs_dirty_inode(struct inode *inode, int flags)
{
	static int noisy = 5;

	if (isReadOnly(inode)) {
		if (!special_file(inode->i_mode) && noisy) {
			/* kernel allows writes to devices on read-only
			 * partitions and may try to mark inode dirty
			 */
			jfs_err("jfs_dirty_inode called on read-only volume");
			jfs_err("Is remount racy?");
			noisy--;
		}
		return;
	}

	set_cflag(COMMIT_Dirty, inode);
}