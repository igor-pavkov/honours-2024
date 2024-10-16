static struct buffer_head *ext4_append(handle_t *handle,
					struct inode *inode,
					ext4_lblk_t *block, int *err)
{
	struct buffer_head *bh;

	*block = inode->i_size >> inode->i_sb->s_blocksize_bits;

	bh = ext4_bread(handle, inode, *block, 1, err);
	if (bh) {
		inode->i_size += inode->i_sb->s_blocksize;
		EXT4_I(inode)->i_disksize = inode->i_size;
		*err = ext4_journal_get_write_access(handle, bh);
		if (*err) {
			brelse(bh);
			bh = NULL;
		}
	}
	return bh;
}