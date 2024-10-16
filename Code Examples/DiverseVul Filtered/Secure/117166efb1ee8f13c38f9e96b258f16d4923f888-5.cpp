int ext4_inline_data_iomap(struct inode *inode, struct iomap *iomap)
{
	__u64 addr;
	int error = -EAGAIN;
	struct ext4_iloc iloc;

	down_read(&EXT4_I(inode)->xattr_sem);
	if (!ext4_has_inline_data(inode))
		goto out;

	error = ext4_get_inode_loc(inode, &iloc);
	if (error)
		goto out;

	addr = (__u64)iloc.bh->b_blocknr << inode->i_sb->s_blocksize_bits;
	addr += (char *)ext4_raw_inode(&iloc) - iloc.bh->b_data;
	addr += offsetof(struct ext4_inode, i_block);

	brelse(iloc.bh);

	iomap->addr = addr;
	iomap->offset = 0;
	iomap->length = min_t(loff_t, ext4_get_inline_size(inode),
			      i_size_read(inode));
	iomap->type = 0;
	iomap->flags = IOMAP_F_DATA_INLINE;

out:
	up_read(&EXT4_I(inode)->xattr_sem);
	return error;
}