static void f2fs_write_failed(struct address_space *mapping, loff_t to)
{
	struct inode *inode = mapping->host;
	loff_t i_size = i_size_read(inode);

	if (to > i_size) {
		truncate_pagecache(inode, i_size);
		truncate_blocks(inode, i_size, true);
	}
}