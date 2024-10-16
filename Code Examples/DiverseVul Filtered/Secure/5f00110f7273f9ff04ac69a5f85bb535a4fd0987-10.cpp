static void shmem_undo_range(struct inode *inode, loff_t lstart, loff_t lend,
								 bool unfalloc)
{
	struct address_space *mapping = inode->i_mapping;
	struct shmem_inode_info *info = SHMEM_I(inode);
	pgoff_t start = (lstart + PAGE_CACHE_SIZE - 1) >> PAGE_CACHE_SHIFT;
	pgoff_t end = (lend + 1) >> PAGE_CACHE_SHIFT;
	unsigned int partial_start = lstart & (PAGE_CACHE_SIZE - 1);
	unsigned int partial_end = (lend + 1) & (PAGE_CACHE_SIZE - 1);
	struct pagevec pvec;
	pgoff_t indices[PAGEVEC_SIZE];
	long nr_swaps_freed = 0;
	pgoff_t index;
	int i;

	if (lend == -1)
		end = -1;	/* unsigned, so actually very big */

	pagevec_init(&pvec, 0);
	index = start;
	while (index < end) {
		pvec.nr = shmem_find_get_pages_and_swap(mapping, index,
				min(end - index, (pgoff_t)PAGEVEC_SIZE),
							pvec.pages, indices);
		if (!pvec.nr)
			break;
		mem_cgroup_uncharge_start();
		for (i = 0; i < pagevec_count(&pvec); i++) {
			struct page *page = pvec.pages[i];

			index = indices[i];
			if (index >= end)
				break;

			if (radix_tree_exceptional_entry(page)) {
				if (unfalloc)
					continue;
				nr_swaps_freed += !shmem_free_swap(mapping,
								index, page);
				continue;
			}

			if (!trylock_page(page))
				continue;
			if (!unfalloc || !PageUptodate(page)) {
				if (page->mapping == mapping) {
					VM_BUG_ON(PageWriteback(page));
					truncate_inode_page(mapping, page);
				}
			}
			unlock_page(page);
		}
		shmem_deswap_pagevec(&pvec);
		pagevec_release(&pvec);
		mem_cgroup_uncharge_end();
		cond_resched();
		index++;
	}

	if (partial_start) {
		struct page *page = NULL;
		shmem_getpage(inode, start - 1, &page, SGP_READ, NULL);
		if (page) {
			unsigned int top = PAGE_CACHE_SIZE;
			if (start > end) {
				top = partial_end;
				partial_end = 0;
			}
			zero_user_segment(page, partial_start, top);
			set_page_dirty(page);
			unlock_page(page);
			page_cache_release(page);
		}
	}
	if (partial_end) {
		struct page *page = NULL;
		shmem_getpage(inode, end, &page, SGP_READ, NULL);
		if (page) {
			zero_user_segment(page, 0, partial_end);
			set_page_dirty(page);
			unlock_page(page);
			page_cache_release(page);
		}
	}
	if (start >= end)
		return;

	index = start;
	for ( ; ; ) {
		cond_resched();
		pvec.nr = shmem_find_get_pages_and_swap(mapping, index,
				min(end - index, (pgoff_t)PAGEVEC_SIZE),
							pvec.pages, indices);
		if (!pvec.nr) {
			if (index == start || unfalloc)
				break;
			index = start;
			continue;
		}
		if ((index == start || unfalloc) && indices[0] >= end) {
			shmem_deswap_pagevec(&pvec);
			pagevec_release(&pvec);
			break;
		}
		mem_cgroup_uncharge_start();
		for (i = 0; i < pagevec_count(&pvec); i++) {
			struct page *page = pvec.pages[i];

			index = indices[i];
			if (index >= end)
				break;

			if (radix_tree_exceptional_entry(page)) {
				if (unfalloc)
					continue;
				nr_swaps_freed += !shmem_free_swap(mapping,
								index, page);
				continue;
			}

			lock_page(page);
			if (!unfalloc || !PageUptodate(page)) {
				if (page->mapping == mapping) {
					VM_BUG_ON(PageWriteback(page));
					truncate_inode_page(mapping, page);
				}
			}
			unlock_page(page);
		}
		shmem_deswap_pagevec(&pvec);
		pagevec_release(&pvec);
		mem_cgroup_uncharge_end();
		index++;
	}

	spin_lock(&info->lock);
	info->swapped -= nr_swaps_freed;
	shmem_recalc_inode(inode);
	spin_unlock(&info->lock);
}