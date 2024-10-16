struct buffer_head *ext4_get_first_inline_block(struct inode *inode,
					struct ext4_dir_entry_2 **parent_de,
					int *retval)
{
	struct ext4_iloc iloc;

	*retval = ext4_get_inode_loc(inode, &iloc);
	if (*retval)
		return NULL;

	*parent_de = (struct ext4_dir_entry_2 *)ext4_raw_inode(&iloc)->i_block;

	return iloc.bh;
}