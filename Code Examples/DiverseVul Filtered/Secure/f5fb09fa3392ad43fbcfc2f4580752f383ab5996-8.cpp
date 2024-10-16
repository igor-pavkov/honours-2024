static int minix_readpage(struct file *file, struct page *page)
{
	return block_read_full_page(page,minix_get_block);
}