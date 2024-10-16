static struct page *nfs_readdir_page_get_locked(struct address_space *mapping,
						pgoff_t index, u64 last_cookie)
{
	struct page *page;

	page = grab_cache_page(mapping, index);
	if (page && !PageUptodate(page)) {
		nfs_readdir_page_init_array(page, last_cookie);
		if (invalidate_inode_pages2_range(mapping, index + 1, -1) < 0)
			nfs_zap_mapping(mapping->host, mapping);
		SetPageUptodate(page);
	}

	return page;
}