static unsigned long set_max_huge_pages(struct hstate *h, unsigned long count,
						nodemask_t *nodes_allowed)
{
	unsigned long min_count, ret;

	if (hstate_is_gigantic(h) && !gigantic_page_supported())
		return h->max_huge_pages;

	/*
	 * Increase the pool size
	 * First take pages out of surplus state.  Then make up the
	 * remaining difference by allocating fresh huge pages.
	 *
	 * We might race with __alloc_buddy_huge_page() here and be unable
	 * to convert a surplus huge page to a normal huge page. That is
	 * not critical, though, it just means the overall size of the
	 * pool might be one hugepage larger than it needs to be, but
	 * within all the constraints specified by the sysctls.
	 */
	spin_lock(&hugetlb_lock);
	while (h->surplus_huge_pages && count > persistent_huge_pages(h)) {
		if (!adjust_pool_surplus(h, nodes_allowed, -1))
			break;
	}

	while (count > persistent_huge_pages(h)) {
		/*
		 * If this allocation races such that we no longer need the
		 * page, free_huge_page will handle it by freeing the page
		 * and reducing the surplus.
		 */
		spin_unlock(&hugetlb_lock);

		/* yield cpu to avoid soft lockup */
		cond_resched();

		if (hstate_is_gigantic(h))
			ret = alloc_fresh_gigantic_page(h, nodes_allowed);
		else
			ret = alloc_fresh_huge_page(h, nodes_allowed);
		spin_lock(&hugetlb_lock);
		if (!ret)
			goto out;

		/* Bail for signals. Probably ctrl-c from user */
		if (signal_pending(current))
			goto out;
	}

	/*
	 * Decrease the pool size
	 * First return free pages to the buddy allocator (being careful
	 * to keep enough around to satisfy reservations).  Then place
	 * pages into surplus state as needed so the pool will shrink
	 * to the desired size as pages become free.
	 *
	 * By placing pages into the surplus state independent of the
	 * overcommit value, we are allowing the surplus pool size to
	 * exceed overcommit. There are few sane options here. Since
	 * __alloc_buddy_huge_page() is checking the global counter,
	 * though, we'll note that we're not allowed to exceed surplus
	 * and won't grow the pool anywhere else. Not until one of the
	 * sysctls are changed, or the surplus pages go out of use.
	 */
	min_count = h->resv_huge_pages + h->nr_huge_pages - h->free_huge_pages;
	min_count = max(count, min_count);
	try_to_free_low(h, min_count, nodes_allowed);
	while (min_count < persistent_huge_pages(h)) {
		if (!free_pool_huge_page(h, nodes_allowed, 0))
			break;
		cond_resched_lock(&hugetlb_lock);
	}
	while (count < persistent_huge_pages(h)) {
		if (!adjust_pool_surplus(h, nodes_allowed, 1))
			break;
	}
out:
	ret = persistent_huge_pages(h);
	spin_unlock(&hugetlb_lock);
	return ret;
}