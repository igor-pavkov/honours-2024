static int jfs_writepage(struct page *page, struct writeback_control *wbc)
{
	return block_write_full_page(page, jfs_get_block, wbc);
}