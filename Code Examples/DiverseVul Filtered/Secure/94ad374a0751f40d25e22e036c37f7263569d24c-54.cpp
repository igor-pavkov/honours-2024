struct page *read_cache_page(struct address_space *mapping,
				pgoff_t index,
				int (*filler)(void *,struct page*),
				void *data)
{
	struct page *page;

	page = read_cache_page_async(mapping, index, filler, data);
	if (IS_ERR(page))
		goto out;
	wait_on_page_locked(page);
	if (!PageUptodate(page)) {
		page_cache_release(page);
		page = ERR_PTR(-EIO);
	}
 out:
	return page;
}