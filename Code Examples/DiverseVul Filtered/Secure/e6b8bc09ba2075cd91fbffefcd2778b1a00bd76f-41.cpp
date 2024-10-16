static int ext4_rename(struct inode *old_dir, struct dentry *old_dentry,
		       struct inode *new_dir, struct dentry *new_dentry)
{
	handle_t *handle;
	struct inode *old_inode, *new_inode;
	struct buffer_head *old_bh, *new_bh, *dir_bh;
	struct ext4_dir_entry_2 *old_de, *new_de;
	int retval;

	old_bh = new_bh = dir_bh = NULL;

	/* Initialize quotas before so that eventual writes go
	 * in separate transaction */
	if (new_dentry->d_inode)
		DQUOT_INIT(new_dentry->d_inode);
	handle = ext4_journal_start(old_dir, 2 *
					EXT4_DATA_TRANS_BLOCKS(old_dir->i_sb) +
					EXT4_INDEX_EXTRA_TRANS_BLOCKS + 2);
	if (IS_ERR(handle))
		return PTR_ERR(handle);

	if (IS_DIRSYNC(old_dir) || IS_DIRSYNC(new_dir))
		ext4_handle_sync(handle);

	old_bh = ext4_find_entry(old_dir, &old_dentry->d_name, &old_de);
	/*
	 *  Check for inode number is _not_ due to possible IO errors.
	 *  We might rmdir the source, keep it as pwd of some process
	 *  and merrily kill the link to whatever was created under the
	 *  same name. Goodbye sticky bit ;-<
	 */
	old_inode = old_dentry->d_inode;
	retval = -ENOENT;
	if (!old_bh || le32_to_cpu(old_de->inode) != old_inode->i_ino)
		goto end_rename;

	new_inode = new_dentry->d_inode;
	new_bh = ext4_find_entry(new_dir, &new_dentry->d_name, &new_de);
	if (new_bh) {
		if (!new_inode) {
			brelse(new_bh);
			new_bh = NULL;
		}
	}
	if (S_ISDIR(old_inode->i_mode)) {
		if (new_inode) {
			retval = -ENOTEMPTY;
			if (!empty_dir(new_inode))
				goto end_rename;
		}
		retval = -EIO;
		dir_bh = ext4_bread(handle, old_inode, 0, 0, &retval);
		if (!dir_bh)
			goto end_rename;
		if (le32_to_cpu(PARENT_INO(dir_bh->b_data)) != old_dir->i_ino)
			goto end_rename;
		retval = -EMLINK;
		if (!new_inode && new_dir != old_dir &&
				new_dir->i_nlink >= EXT4_LINK_MAX)
			goto end_rename;
	}
	if (!new_bh) {
		retval = ext4_add_entry(handle, new_dentry, old_inode);
		if (retval)
			goto end_rename;
	} else {
		BUFFER_TRACE(new_bh, "get write access");
		ext4_journal_get_write_access(handle, new_bh);
		new_de->inode = cpu_to_le32(old_inode->i_ino);
		if (EXT4_HAS_INCOMPAT_FEATURE(new_dir->i_sb,
					      EXT4_FEATURE_INCOMPAT_FILETYPE))
			new_de->file_type = old_de->file_type;
		new_dir->i_version++;
		new_dir->i_ctime = new_dir->i_mtime =
					ext4_current_time(new_dir);
		ext4_mark_inode_dirty(handle, new_dir);
		BUFFER_TRACE(new_bh, "call ext4_handle_dirty_metadata");
		ext4_handle_dirty_metadata(handle, new_dir, new_bh);
		brelse(new_bh);
		new_bh = NULL;
	}

	/*
	 * Like most other Unix systems, set the ctime for inodes on a
	 * rename.
	 */
	old_inode->i_ctime = ext4_current_time(old_inode);
	ext4_mark_inode_dirty(handle, old_inode);

	/*
	 * ok, that's it
	 */
	if (le32_to_cpu(old_de->inode) != old_inode->i_ino ||
	    old_de->name_len != old_dentry->d_name.len ||
	    strncmp(old_de->name, old_dentry->d_name.name, old_de->name_len) ||
	    (retval = ext4_delete_entry(handle, old_dir,
					old_de, old_bh)) == -ENOENT) {
		/* old_de could have moved from under us during htree split, so
		 * make sure that we are deleting the right entry.  We might
		 * also be pointing to a stale entry in the unused part of
		 * old_bh so just checking inum and the name isn't enough. */
		struct buffer_head *old_bh2;
		struct ext4_dir_entry_2 *old_de2;

		old_bh2 = ext4_find_entry(old_dir, &old_dentry->d_name, &old_de2);
		if (old_bh2) {
			retval = ext4_delete_entry(handle, old_dir,
						   old_de2, old_bh2);
			brelse(old_bh2);
		}
	}
	if (retval) {
		ext4_warning(old_dir->i_sb, "ext4_rename",
				"Deleting old file (%lu), %d, error=%d",
				old_dir->i_ino, old_dir->i_nlink, retval);
	}

	if (new_inode) {
		ext4_dec_count(handle, new_inode);
		new_inode->i_ctime = ext4_current_time(new_inode);
	}
	old_dir->i_ctime = old_dir->i_mtime = ext4_current_time(old_dir);
	ext4_update_dx_flag(old_dir);
	if (dir_bh) {
		BUFFER_TRACE(dir_bh, "get_write_access");
		ext4_journal_get_write_access(handle, dir_bh);
		PARENT_INO(dir_bh->b_data) = cpu_to_le32(new_dir->i_ino);
		BUFFER_TRACE(dir_bh, "call ext4_handle_dirty_metadata");
		ext4_handle_dirty_metadata(handle, old_dir, dir_bh);
		ext4_dec_count(handle, old_dir);
		if (new_inode) {
			/* checked empty_dir above, can't have another parent,
			 * ext4_dec_count() won't work for many-linked dirs */
			new_inode->i_nlink = 0;
		} else {
			ext4_inc_count(handle, new_dir);
			ext4_update_dx_flag(new_dir);
			ext4_mark_inode_dirty(handle, new_dir);
		}
	}
	ext4_mark_inode_dirty(handle, old_dir);
	if (new_inode) {
		ext4_mark_inode_dirty(handle, new_inode);
		if (!new_inode->i_nlink)
			ext4_orphan_add(handle, new_inode);
	}
	retval = 0;

end_rename:
	brelse(dir_bh);
	brelse(old_bh);
	brelse(new_bh);
	ext4_journal_stop(handle);
	return retval;
}