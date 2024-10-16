int add_to_page_cache_locked(struct page *page, struct address_space *mapping,
		pgoff_t offset, gfp_t gfp_mask)
{
	int error;

	VM_BUG_ON(!PageLocked(page));

	error = mem_cgroup_cache_charge(page, current->mm,
					gfp_mask & ~__GFP_HIGHMEM);
	if (error)
		goto out;

	error = radix_tree_preload(gfp_mask & ~__GFP_HIGHMEM);
	if (error == 0) {
		page_cache_get(page);
		page->mapping = mapping;
		page->index = offset;

		spin_lock_irq(&mapping->tree_lock);
		error = radix_tree_insert(&mapping->page_tree, offset, page);
		if (likely(!error)) {
			mapping->nrpages++;
			__inc_zone_page_state(page, NR_FILE_PAGES);
		} else {
			page->mapping = NULL;
			mem_cgroup_uncharge_cache_page(page);
			page_cache_release(page);
		}

		spin_unlock_irq(&mapping->tree_lock);
		radix_tree_preload_end();
	} else
		mem_cgroup_uncharge_cache_page(page);
out:
	return error;
}