int isolate_movable_page(struct page *page, isolate_mode_t mode)
{
	struct address_space *mapping;

	/*
	 * Avoid burning cycles with pages that are yet under __free_pages(),
	 * or just got freed under us.
	 *
	 * In case we 'win' a race for a movable page being freed under us and
	 * raise its refcount preventing __free_pages() from doing its job
	 * the put_page() at the end of this block will take care of
	 * release this page, thus avoiding a nasty leakage.
	 */
	if (unlikely(!get_page_unless_zero(page)))
		goto out;

	/*
	 * Check PageMovable before holding a PG_lock because page's owner
	 * assumes anybody doesn't touch PG_lock of newly allocated page
	 * so unconditionally grapping the lock ruins page's owner side.
	 */
	if (unlikely(!__PageMovable(page)))
		goto out_putpage;
	/*
	 * As movable pages are not isolated from LRU lists, concurrent
	 * compaction threads can race against page migration functions
	 * as well as race against the releasing a page.
	 *
	 * In order to avoid having an already isolated movable page
	 * being (wrongly) re-isolated while it is under migration,
	 * or to avoid attempting to isolate pages being released,
	 * lets be sure we have the page lock
	 * before proceeding with the movable page isolation steps.
	 */
	if (unlikely(!trylock_page(page)))
		goto out_putpage;

	if (!PageMovable(page) || PageIsolated(page))
		goto out_no_isolated;

	mapping = page_mapping(page);
	VM_BUG_ON_PAGE(!mapping, page);

	if (!mapping->a_ops->isolate_page(page, mode))
		goto out_no_isolated;

	/* Driver shouldn't use PG_isolated bit of page->flags */
	WARN_ON_ONCE(PageIsolated(page));
	__SetPageIsolated(page);
	unlock_page(page);

	return 0;

out_no_isolated:
	unlock_page(page);
out_putpage:
	put_page(page);
out:
	return -EBUSY;
}