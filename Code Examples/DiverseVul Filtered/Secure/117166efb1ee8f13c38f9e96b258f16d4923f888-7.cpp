static void *ext4_get_inline_xattr_pos(struct inode *inode,
				       struct ext4_iloc *iloc)
{
	struct ext4_xattr_entry *entry;
	struct ext4_xattr_ibody_header *header;

	BUG_ON(!EXT4_I(inode)->i_inline_off);

	header = IHDR(inode, ext4_raw_inode(iloc));
	entry = (struct ext4_xattr_entry *)((void *)ext4_raw_inode(iloc) +
					    EXT4_I(inode)->i_inline_off);

	return (void *)IFIRST(header) + le16_to_cpu(entry->e_value_offs);
}