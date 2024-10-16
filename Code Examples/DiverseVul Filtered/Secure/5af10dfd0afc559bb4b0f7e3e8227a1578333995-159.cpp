static void destroy_compound_gigantic_page(struct page *page,
					unsigned int order)
{
	int i;
	int nr_pages = 1 << order;
	struct page *p = page + 1;

	atomic_set(compound_mapcount_ptr(page), 0);
	for (i = 1; i < nr_pages; i++, p = mem_map_next(p, page, i)) {
		clear_compound_head(p);
		set_page_refcounted(p);
	}

	set_compound_order(page, 0);
	__ClearPageHead(page);
}