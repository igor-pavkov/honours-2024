static void ext4_write_inline_data(struct inode *inode, struct ext4_iloc *iloc,
				   void *buffer, loff_t pos, unsigned int len)
{
	struct ext4_xattr_entry *entry;
	struct ext4_xattr_ibody_header *header;
	struct ext4_inode *raw_inode;
	int cp_len = 0;

	if (unlikely(ext4_forced_shutdown(EXT4_SB(inode->i_sb))))
		return;

	BUG_ON(!EXT4_I(inode)->i_inline_off);
	BUG_ON(pos + len > EXT4_I(inode)->i_inline_size);

	raw_inode = ext4_raw_inode(iloc);
	buffer += pos;

	if (pos < EXT4_MIN_INLINE_DATA_SIZE) {
		cp_len = pos + len > EXT4_MIN_INLINE_DATA_SIZE ?
			 EXT4_MIN_INLINE_DATA_SIZE - pos : len;
		memcpy((void *)raw_inode->i_block + pos, buffer, cp_len);

		len -= cp_len;
		buffer += cp_len;
		pos += cp_len;
	}

	if (!len)
		return;

	pos -= EXT4_MIN_INLINE_DATA_SIZE;
	header = IHDR(inode, raw_inode);
	entry = (struct ext4_xattr_entry *)((void *)raw_inode +
					    EXT4_I(inode)->i_inline_off);

	memcpy((void *)IFIRST(header) + le16_to_cpu(entry->e_value_offs) + pos,
	       buffer, len);
}