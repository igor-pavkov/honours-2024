static void nfs_readdir_free_pages(struct page **pages, size_t npages)
{
	while (npages--)
		put_page(pages[npages]);
	kfree(pages);
}