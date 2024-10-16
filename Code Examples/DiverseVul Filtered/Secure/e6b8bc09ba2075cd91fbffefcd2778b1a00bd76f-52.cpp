static int ext4_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	handle_t *handle;
	struct inode *inode;
	struct buffer_head *dir_block;
	struct ext4_dir_entry_2 *de;
	int err, retries = 0;

	if (EXT4_DIR_LINK_MAX(dir))
		return -EMLINK;

retry:
	handle = ext4_journal_start(dir, EXT4_DATA_TRANS_BLOCKS(dir->i_sb) +
					EXT4_INDEX_EXTRA_TRANS_BLOCKS + 3 +
					2*EXT4_QUOTA_INIT_BLOCKS(dir->i_sb));
	if (IS_ERR(handle))
		return PTR_ERR(handle);

	if (IS_DIRSYNC(dir))
		ext4_handle_sync(handle);

	inode = ext4_new_inode(handle, dir, S_IFDIR | mode);
	err = PTR_ERR(inode);
	if (IS_ERR(inode))
		goto out_stop;

	inode->i_op = &ext4_dir_inode_operations;
	inode->i_fop = &ext4_dir_operations;
	inode->i_size = EXT4_I(inode)->i_disksize = inode->i_sb->s_blocksize;
	dir_block = ext4_bread(handle, inode, 0, 1, &err);
	if (!dir_block)
		goto out_clear_inode;
	BUFFER_TRACE(dir_block, "get_write_access");
	ext4_journal_get_write_access(handle, dir_block);
	de = (struct ext4_dir_entry_2 *) dir_block->b_data;
	de->inode = cpu_to_le32(inode->i_ino);
	de->name_len = 1;
	de->rec_len = ext4_rec_len_to_disk(EXT4_DIR_REC_LEN(de->name_len));
	strcpy(de->name, ".");
	ext4_set_de_type(dir->i_sb, de, S_IFDIR);
	de = ext4_next_entry(de);
	de->inode = cpu_to_le32(dir->i_ino);
	de->rec_len = ext4_rec_len_to_disk(inode->i_sb->s_blocksize -
						EXT4_DIR_REC_LEN(1));
	de->name_len = 2;
	strcpy(de->name, "..");
	ext4_set_de_type(dir->i_sb, de, S_IFDIR);
	inode->i_nlink = 2;
	BUFFER_TRACE(dir_block, "call ext4_handle_dirty_metadata");
	ext4_handle_dirty_metadata(handle, dir, dir_block);
	brelse(dir_block);
	ext4_mark_inode_dirty(handle, inode);
	err = ext4_add_entry(handle, dentry, inode);
	if (err) {
out_clear_inode:
		clear_nlink(inode);
		unlock_new_inode(inode);
		ext4_mark_inode_dirty(handle, inode);
		iput(inode);
		goto out_stop;
	}
	ext4_inc_count(handle, dir);
	ext4_update_dx_flag(dir);
	ext4_mark_inode_dirty(handle, dir);
	d_instantiate(dentry, inode);
	unlock_new_inode(inode);
out_stop:
	ext4_journal_stop(handle);
	if (err == -ENOSPC && ext4_should_retry_alloc(dir->i_sb, &retries))
		goto retry;
	return err;
}