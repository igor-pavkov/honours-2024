static int udf_writepage(struct page *page, struct writeback_control *wbc)
{
	return block_write_full_page(page, udf_get_block, wbc);
}