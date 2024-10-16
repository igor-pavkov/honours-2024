static struct page *nfs_readdir_page_get_next(struct address_space *mapping,
					      pgoff_t index, u64 cookie)
{
	struct page *page;

	page = nfs_readdir_page_get_locked(mapping, index, cookie);
	if (page) {
		if (nfs_readdir_page_last_cookie(page) == cookie)
			return page;
		nfs_readdir_page_unlock_and_put(page);
	}
	return NULL;
}