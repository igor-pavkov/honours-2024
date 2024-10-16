static int ext4_read_inline_data(struct inode *inode, void *buffer,
				 unsigned int len,
				 struct ext4_iloc *iloc)
{
	struct ext4_xattr_entry *entry;
	struct ext4_xattr_ibody_header *header;
	int cp_len = 0;
	struct ext4_inode *raw_inode;

	if (!len)
		return 0;

	BUG_ON(len > EXT4_I(inode)->i_inline_size);

	cp_len = len < EXT4_MIN_INLINE_DATA_SIZE ?
			len : EXT4_MIN_INLINE_DATA_SIZE;

	raw_inode = ext4_raw_inode(iloc);
	memcpy(buffer, (void *)(raw_inode->i_block), cp_len);

	len -= cp_len;
	buffer += cp_len;

	if (!len)
		goto out;

	header = IHDR(inode, raw_inode);
	entry = (struct ext4_xattr_entry *)((void *)raw_inode +
					    EXT4_I(inode)->i_inline_off);
	len = min_t(unsigned int, len,
		    (unsigned int)le32_to_cpu(entry->e_value_size));

	memcpy(buffer,
	       (void *)IFIRST(header) + le16_to_cpu(entry->e_value_offs), len);
	cp_len += len;

out:
	return cp_len;
}