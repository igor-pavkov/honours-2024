bool empty_inline_dir(struct inode *dir, int *has_inline_data)
{
	int err, inline_size;
	struct ext4_iloc iloc;
	void *inline_pos;
	unsigned int offset;
	struct ext4_dir_entry_2 *de;
	bool ret = true;

	err = ext4_get_inode_loc(dir, &iloc);
	if (err) {
		EXT4_ERROR_INODE(dir, "error %d getting inode %lu block",
				 err, dir->i_ino);
		return true;
	}

	down_read(&EXT4_I(dir)->xattr_sem);
	if (!ext4_has_inline_data(dir)) {
		*has_inline_data = 0;
		goto out;
	}

	de = (struct ext4_dir_entry_2 *)ext4_raw_inode(&iloc)->i_block;
	if (!le32_to_cpu(de->inode)) {
		ext4_warning(dir->i_sb,
			     "bad inline directory (dir #%lu) - no `..'",
			     dir->i_ino);
		ret = true;
		goto out;
	}

	offset = EXT4_INLINE_DOTDOT_SIZE;
	while (offset < dir->i_size) {
		de = ext4_get_inline_entry(dir, &iloc, offset,
					   &inline_pos, &inline_size);
		if (ext4_check_dir_entry(dir, NULL, de,
					 iloc.bh, inline_pos,
					 inline_size, offset)) {
			ext4_warning(dir->i_sb,
				     "bad inline directory (dir #%lu) - "
				     "inode %u, rec_len %u, name_len %d"
				     "inline size %d",
				     dir->i_ino, le32_to_cpu(de->inode),
				     le16_to_cpu(de->rec_len), de->name_len,
				     inline_size);
			ret = true;
			goto out;
		}
		if (le32_to_cpu(de->inode)) {
			ret = false;
			goto out;
		}
		offset += ext4_rec_len_from_disk(de->rec_len, inline_size);
	}

out:
	up_read(&EXT4_I(dir)->xattr_sem);
	brelse(iloc.bh);
	return ret;
}