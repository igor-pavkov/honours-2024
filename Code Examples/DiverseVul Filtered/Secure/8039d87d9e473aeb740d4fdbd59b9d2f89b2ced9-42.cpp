static struct page *extent_same_get_page(struct inode *inode, pgoff_t index)
{
	struct page *page;
	struct extent_io_tree *tree = &BTRFS_I(inode)->io_tree;

	page = grab_cache_page(inode->i_mapping, index);
	if (!page)
		return NULL;

	if (!PageUptodate(page)) {
		if (extent_read_full_page_nolock(tree, page, btrfs_get_extent,
						 0))
			return NULL;
		lock_page(page);
		if (!PageUptodate(page)) {
			unlock_page(page);
			page_cache_release(page);
			return NULL;
		}
	}
	unlock_page(page);

	return page;
}