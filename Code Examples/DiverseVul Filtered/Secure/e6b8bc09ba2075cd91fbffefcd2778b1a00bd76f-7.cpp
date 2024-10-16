static int add_dirent_to_buf(handle_t *handle, struct dentry *dentry,
			     struct inode *inode, struct ext4_dir_entry_2 *de,
			     struct buffer_head *bh)
{
	struct inode	*dir = dentry->d_parent->d_inode;
	const char	*name = dentry->d_name.name;
	int		namelen = dentry->d_name.len;
	unsigned int	offset = 0;
	unsigned short	reclen;
	int		nlen, rlen, err;
	char		*top;

	reclen = EXT4_DIR_REC_LEN(namelen);
	if (!de) {
		de = (struct ext4_dir_entry_2 *)bh->b_data;
		top = bh->b_data + dir->i_sb->s_blocksize - reclen;
		while ((char *) de <= top) {
			if (!ext4_check_dir_entry("ext4_add_entry", dir, de,
						  bh, offset)) {
				brelse(bh);
				return -EIO;
			}
			if (ext4_match(namelen, name, de)) {
				brelse(bh);
				return -EEXIST;
			}
			nlen = EXT4_DIR_REC_LEN(de->name_len);
			rlen = ext4_rec_len_from_disk(de->rec_len);
			if ((de->inode? rlen - nlen: rlen) >= reclen)
				break;
			de = (struct ext4_dir_entry_2 *)((char *)de + rlen);
			offset += rlen;
		}
		if ((char *) de > top)
			return -ENOSPC;
	}
	BUFFER_TRACE(bh, "get_write_access");
	err = ext4_journal_get_write_access(handle, bh);
	if (err) {
		ext4_std_error(dir->i_sb, err);
		brelse(bh);
		return err;
	}

	/* By now the buffer is marked for journaling */
	nlen = EXT4_DIR_REC_LEN(de->name_len);
	rlen = ext4_rec_len_from_disk(de->rec_len);
	if (de->inode) {
		struct ext4_dir_entry_2 *de1 = (struct ext4_dir_entry_2 *)((char *)de + nlen);
		de1->rec_len = ext4_rec_len_to_disk(rlen - nlen);
		de->rec_len = ext4_rec_len_to_disk(nlen);
		de = de1;
	}
	de->file_type = EXT4_FT_UNKNOWN;
	if (inode) {
		de->inode = cpu_to_le32(inode->i_ino);
		ext4_set_de_type(dir->i_sb, de, inode->i_mode);
	} else
		de->inode = 0;
	de->name_len = namelen;
	memcpy(de->name, name, namelen);
	/*
	 * XXX shouldn't update any times until successful
	 * completion of syscall, but too many callers depend
	 * on this.
	 *
	 * XXX similarly, too many callers depend on
	 * ext4_new_inode() setting the times, but error
	 * recovery deletes the inode, so the worst that can
	 * happen is that the times are slightly out of date
	 * and/or different from the directory change time.
	 */
	dir->i_mtime = dir->i_ctime = ext4_current_time(dir);
	ext4_update_dx_flag(dir);
	dir->i_version++;
	ext4_mark_inode_dirty(handle, dir);
	BUFFER_TRACE(bh, "call ext4_handle_dirty_metadata");
	err = ext4_handle_dirty_metadata(handle, dir, bh);
	if (err)
		ext4_std_error(dir->i_sb, err);
	brelse(bh);
	return 0;
}