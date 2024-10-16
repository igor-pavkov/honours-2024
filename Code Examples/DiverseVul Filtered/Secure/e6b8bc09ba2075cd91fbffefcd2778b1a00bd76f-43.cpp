static int ext4_symlink(struct inode *dir,
			struct dentry *dentry, const char *symname)
{
	handle_t *handle;
	struct inode *inode;
	int l, err, retries = 0;

	l = strlen(symname)+1;
	if (l > dir->i_sb->s_blocksize)
		return -ENAMETOOLONG;

retry:
	handle = ext4_journal_start(dir, EXT4_DATA_TRANS_BLOCKS(dir->i_sb) +
					EXT4_INDEX_EXTRA_TRANS_BLOCKS + 5 +
					2*EXT4_QUOTA_INIT_BLOCKS(dir->i_sb));
	if (IS_ERR(handle))
		return PTR_ERR(handle);

	if (IS_DIRSYNC(dir))
		ext4_handle_sync(handle);

	inode = ext4_new_inode(handle, dir, S_IFLNK|S_IRWXUGO);
	err = PTR_ERR(inode);
	if (IS_ERR(inode))
		goto out_stop;

	if (l > sizeof(EXT4_I(inode)->i_data)) {
		inode->i_op = &ext4_symlink_inode_operations;
		ext4_set_aops(inode);
		/*
		 * page_symlink() calls into ext4_prepare/commit_write.
		 * We have a transaction open.  All is sweetness.  It also sets
		 * i_size in generic_commit_write().
		 */
		err = __page_symlink(inode, symname, l, 1);
		if (err) {
			clear_nlink(inode);
			unlock_new_inode(inode);
			ext4_mark_inode_dirty(handle, inode);
			iput(inode);
			goto out_stop;
		}
	} else {
		/* clear the extent format for fast symlink */
		EXT4_I(inode)->i_flags &= ~EXT4_EXTENTS_FL;
		inode->i_op = &ext4_fast_symlink_inode_operations;
		memcpy((char *)&EXT4_I(inode)->i_data, symname, l);
		inode->i_size = l-1;
	}
	EXT4_I(inode)->i_disksize = inode->i_size;
	err = ext4_add_nondir(handle, dentry, inode);
out_stop:
	ext4_journal_stop(handle);
	if (err == -ENOSPC && ext4_should_retry_alloc(dir->i_sb, &retries))
		goto retry;
	return err;
}