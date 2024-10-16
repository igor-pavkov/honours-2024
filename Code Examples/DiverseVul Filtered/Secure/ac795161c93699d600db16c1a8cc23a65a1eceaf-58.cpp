static struct page **nfs_readdir_alloc_pages(size_t npages)
{
	struct page **pages;
	size_t i;

	pages = kmalloc_array(npages, sizeof(*pages), GFP_KERNEL);
	if (!pages)
		return NULL;
	for (i = 0; i < npages; i++) {
		struct page *page = alloc_page(GFP_KERNEL);
		if (page == NULL)
			goto out_freepages;
		pages[i] = page;
	}
	return pages;

out_freepages:
	nfs_readdir_free_pages(pages, i);
	return NULL;
}