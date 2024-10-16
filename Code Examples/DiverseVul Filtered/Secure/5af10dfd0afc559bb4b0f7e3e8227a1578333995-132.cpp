static int gather_surplus_pages(struct hstate *h, int delta)
{
	struct list_head surplus_list;
	struct page *page, *tmp;
	int ret, i;
	int needed, allocated;
	bool alloc_ok = true;

	needed = (h->resv_huge_pages + delta) - h->free_huge_pages;
	if (needed <= 0) {
		h->resv_huge_pages += delta;
		return 0;
	}

	allocated = 0;
	INIT_LIST_HEAD(&surplus_list);

	ret = -ENOMEM;
retry:
	spin_unlock(&hugetlb_lock);
	for (i = 0; i < needed; i++) {
		page = __alloc_buddy_huge_page(h, htlb_alloc_mask(h),
				NUMA_NO_NODE, NULL);
		if (!page) {
			alloc_ok = false;
			break;
		}
		list_add(&page->lru, &surplus_list);
		cond_resched();
	}
	allocated += i;

	/*
	 * After retaking hugetlb_lock, we need to recalculate 'needed'
	 * because either resv_huge_pages or free_huge_pages may have changed.
	 */
	spin_lock(&hugetlb_lock);
	needed = (h->resv_huge_pages + delta) -
			(h->free_huge_pages + allocated);
	if (needed > 0) {
		if (alloc_ok)
			goto retry;
		/*
		 * We were not able to allocate enough pages to
		 * satisfy the entire reservation so we free what
		 * we've allocated so far.
		 */
		goto free;
	}
	/*
	 * The surplus_list now contains _at_least_ the number of extra pages
	 * needed to accommodate the reservation.  Add the appropriate number
	 * of pages to the hugetlb pool and free the extras back to the buddy
	 * allocator.  Commit the entire reservation here to prevent another
	 * process from stealing the pages as they are added to the pool but
	 * before they are reserved.
	 */
	needed += allocated;
	h->resv_huge_pages += delta;
	ret = 0;

	/* Free the needed pages to the hugetlb pool */
	list_for_each_entry_safe(page, tmp, &surplus_list, lru) {
		if ((--needed) < 0)
			break;
		/*
		 * This page is now managed by the hugetlb allocator and has
		 * no users -- drop the buddy allocator's reference.
		 */
		put_page_testzero(page);
		VM_BUG_ON_PAGE(page_count(page), page);
		enqueue_huge_page(h, page);
	}
free:
	spin_unlock(&hugetlb_lock);

	/* Free unnecessary surplus pages to the buddy allocator */
	list_for_each_entry_safe(page, tmp, &surplus_list, lru)
		put_page(page);
	spin_lock(&hugetlb_lock);

	return ret;
}