static int shmem_add_to_page_cache(struct page *page,
				   struct address_space *mapping,
				   pgoff_t index, gfp_t gfp, void *expected)
{
	int error;

	VM_BUG_ON(!PageLocked(page));
	VM_BUG_ON(!PageSwapBacked(page));

	page_cache_get(page);
	page->mapping = mapping;
	page->index = index;

	spin_lock_irq(&mapping->tree_lock);
	if (!expected)
		error = radix_tree_insert(&mapping->page_tree, index, page);
	else
		error = shmem_radix_tree_replace(mapping, index, expected,
								 page);
	if (!error) {
		mapping->nrpages++;
		__inc_zone_page_state(page, NR_FILE_PAGES);
		__inc_zone_page_state(page, NR_SHMEM);
		spin_unlock_irq(&mapping->tree_lock);
	} else {
		page->mapping = NULL;
		spin_unlock_irq(&mapping->tree_lock);
		page_cache_release(page);
	}
	return error;
}