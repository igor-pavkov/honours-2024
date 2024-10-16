static int ftrace_allocate_records(struct ftrace_page *pg, int count)
{
	int order;
	int cnt;

	if (WARN_ON(!count))
		return -EINVAL;

	order = get_count_order(DIV_ROUND_UP(count, ENTRIES_PER_PAGE));

	/*
	 * We want to fill as much as possible. No more than a page
	 * may be empty.
	 */
	while ((PAGE_SIZE << order) / ENTRY_SIZE >= count + ENTRIES_PER_PAGE)
		order--;

 again:
	pg->records = (void *)__get_free_pages(GFP_KERNEL | __GFP_ZERO, order);

	if (!pg->records) {
		/* if we can't allocate this size, try something smaller */
		if (!order)
			return -ENOMEM;
		order >>= 1;
		goto again;
	}

	cnt = (PAGE_SIZE << order) / ENTRY_SIZE;
	pg->size = cnt;

	if (cnt > count)
		cnt = count;

	return cnt;
}