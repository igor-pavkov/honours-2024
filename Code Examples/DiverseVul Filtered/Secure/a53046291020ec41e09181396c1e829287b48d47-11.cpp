static int jfs_readpage(struct file *file, struct page *page)
{
	return mpage_readpage(page, jfs_get_block);
}