static int ext4_add_entry(handle_t *handle, struct dentry *dentry,
			  struct inode *inode)
{
	struct inode *dir = dentry->d_parent->d_inode;
	struct buffer_head *bh;
	struct ext4_dir_entry_2 *de;
	struct super_block *sb;
	int	retval;
	int	dx_fallback=0;
	unsigned blocksize;
	ext4_lblk_t block, blocks;

	sb = dir->i_sb;
	blocksize = sb->s_blocksize;
	if (!dentry->d_name.len)
		return -EINVAL;
	if (is_dx(dir)) {
		retval = ext4_dx_add_entry(handle, dentry, inode);
		if (!retval || (retval != ERR_BAD_DX_DIR))
			return retval;
		EXT4_I(dir)->i_flags &= ~EXT4_INDEX_FL;
		dx_fallback++;
		ext4_mark_inode_dirty(handle, dir);
	}
	blocks = dir->i_size >> sb->s_blocksize_bits;
	for (block = 0; block < blocks; block++) {
		bh = ext4_bread(handle, dir, block, 0, &retval);
		if(!bh)
			return retval;
		retval = add_dirent_to_buf(handle, dentry, inode, NULL, bh);
		if (retval != -ENOSPC)
			return retval;

		if (blocks == 1 && !dx_fallback &&
		    EXT4_HAS_COMPAT_FEATURE(sb, EXT4_FEATURE_COMPAT_DIR_INDEX))
			return make_indexed_dir(handle, dentry, inode, bh);
		brelse(bh);
	}
	bh = ext4_append(handle, dir, &block, &retval);
	if (!bh)
		return retval;
	de = (struct ext4_dir_entry_2 *) bh->b_data;
	de->inode = 0;
	de->rec_len = ext4_rec_len_to_disk(blocksize);
	return add_dirent_to_buf(handle, dentry, inode, de, bh);
}