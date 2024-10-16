struct page *__grab_cache_page(struct address_space *mapping, pgoff_t index)
{
	int status;
	struct page *page;
repeat:
	page = find_lock_page(mapping, index);
	if (likely(page))
		return page;

	page = page_cache_alloc(mapping);
	if (!page)
		return NULL;
	status = add_to_page_cache_lru(page, mapping, index, GFP_KERNEL);
	if (unlikely(status)) {
		page_cache_release(page);
		if (status == -EEXIST)
			goto repeat;
		return NULL;
	}
	return page;
}