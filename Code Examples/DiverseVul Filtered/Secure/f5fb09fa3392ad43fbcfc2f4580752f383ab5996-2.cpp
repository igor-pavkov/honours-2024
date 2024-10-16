static int minix_writepage(struct page *page, struct writeback_control *wbc)
{
	return block_write_full_page(page, minix_get_block, wbc);
}