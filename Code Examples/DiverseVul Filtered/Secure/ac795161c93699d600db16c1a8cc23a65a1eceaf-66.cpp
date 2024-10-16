static void nfs_readdir_page_unlock_and_put(struct page *page)
{
	unlock_page(page);
	put_page(page);
}