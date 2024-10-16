static int gather_extent_pages(struct inode *inode, struct page **pages,
			       int num_pages, u64 off)
{
	int i;
	pgoff_t index = off >> PAGE_CACHE_SHIFT;

	for (i = 0; i < num_pages; i++) {
		pages[i] = extent_same_get_page(inode, index + i);
		if (!pages[i])
			return -ENOMEM;
	}
	return 0;
}