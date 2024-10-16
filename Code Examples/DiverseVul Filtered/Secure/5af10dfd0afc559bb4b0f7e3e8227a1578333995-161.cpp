static void prep_compound_gigantic_page(struct page *page, unsigned int order)
{
	int i;
	int nr_pages = 1 << order;
	struct page *p = page + 1;

	/* we rely on prep_new_huge_page to set the destructor */
	set_compound_order(page, order);
	__ClearPageReserved(page);
	__SetPageHead(page);
	for (i = 1; i < nr_pages; i++, p = mem_map_next(p, page, i)) {
		/*
		 * For gigantic hugepages allocated through bootmem at
		 * boot, it's safer to be consistent with the not-gigantic
		 * hugepages and clear the PG_reserved bit from all tail pages
		 * too.  Otherwse drivers using get_user_pages() to access tail
		 * pages may get the reference counting wrong if they see
		 * PG_reserved set on a tail page (despite the head page not
		 * having PG_reserved set).  Enforcing this consistency between
		 * head and tail pages allows drivers to optimize away a check
		 * on the head page when they need know if put_page() is needed
		 * after get_user_pages().
		 */
		__ClearPageReserved(p);
		set_page_count(p, 0);
		set_compound_head(p, page);
	}
	atomic_set(compound_mapcount_ptr(page), -1);
}