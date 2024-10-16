static int ext4_mknod(struct inode *dir, struct dentry *dentry,
		      int mode, dev_t rdev)
{
	handle_t *handle;
	struct inode *inode;
	int err, retries = 0;

	if (!new_valid_dev(rdev))
		return -EINVAL;

retry:
	handle = ext4_journal_start(dir, EXT4_DATA_TRANS_BLOCKS(dir->i_sb) +
					EXT4_INDEX_EXTRA_TRANS_BLOCKS + 3 +
					2*EXT4_QUOTA_INIT_BLOCKS(dir->i_sb));
	if (IS_ERR(handle))
		return PTR_ERR(handle);

	if (IS_DIRSYNC(dir))
		ext4_handle_sync(handle);

	inode = ext4_new_inode(handle, dir, mode);
	err = PTR_ERR(inode);
	if (!IS_ERR(inode)) {
		init_special_inode(inode, inode->i_mode, rdev);
#ifdef CONFIG_EXT4_FS_XATTR
		inode->i_op = &ext4_special_inode_operations;
#endif
		err = ext4_add_nondir(handle, dentry, inode);
	}
	ext4_journal_stop(handle);
	if (err == -ENOSPC && ext4_should_retry_alloc(dir->i_sb, &retries))
		goto retry;
	return err;
}