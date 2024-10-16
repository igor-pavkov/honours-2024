static void nfs_readdir_page_array_free(struct page *page)
{
	if (page) {
		nfs_readdir_clear_array(page);
		put_page(page);
	}
}