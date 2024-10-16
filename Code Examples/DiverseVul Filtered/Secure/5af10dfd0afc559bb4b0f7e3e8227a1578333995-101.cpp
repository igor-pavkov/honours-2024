static void update_and_free_page(struct hstate *h, struct page *page)
{
	int i;

	if (hstate_is_gigantic(h) && !gigantic_page_supported())
		return;

	h->nr_huge_pages--;
	h->nr_huge_pages_node[page_to_nid(page)]--;
	for (i = 0; i < pages_per_huge_page(h); i++) {
		page[i].flags &= ~(1 << PG_locked | 1 << PG_error |
				1 << PG_referenced | 1 << PG_dirty |
				1 << PG_active | 1 << PG_private |
				1 << PG_writeback);
	}
	VM_BUG_ON_PAGE(hugetlb_cgroup_from_page(page), page);
	set_compound_page_dtor(page, NULL_COMPOUND_DTOR);
	set_page_refcounted(page);
	if (hstate_is_gigantic(h)) {
		destroy_compound_gigantic_page(page, huge_page_order(h));
		free_gigantic_page(page, huge_page_order(h));
	} else {
		__free_pages(page, huge_page_order(h));
	}
}