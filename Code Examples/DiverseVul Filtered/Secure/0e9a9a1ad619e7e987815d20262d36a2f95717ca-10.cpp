static struct buffer_head *ext4_get_first_dir_block(handle_t *handle,
					struct inode *inode,
					int *retval,
					struct ext4_dir_entry_2 **parent_de,
					int *inlined)
{
	struct buffer_head *bh;

	if (!ext4_has_inline_data(inode)) {
		if (!(bh = ext4_bread(handle, inode, 0, 0, retval))) {
			if (!*retval) {
				*retval = -EIO;
				ext4_error(inode->i_sb,
					   "Directory hole detected on inode %lu\n",
					   inode->i_ino);
			}
			return NULL;
		}
		*parent_de = ext4_next_entry(
					(struct ext4_dir_entry_2 *)bh->b_data,
					inode->i_sb->s_blocksize);
		return bh;
	}

	*inlined = 1;
	return ext4_get_first_inline_block(inode, parent_de, retval);
}