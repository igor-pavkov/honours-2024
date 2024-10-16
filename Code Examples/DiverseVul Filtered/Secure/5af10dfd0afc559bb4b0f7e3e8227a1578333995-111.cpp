static struct page *alloc_fresh_gigantic_page_node(struct hstate *h, int nid)
{
	struct page *page;

	page = alloc_gigantic_page(nid, huge_page_order(h));
	if (page) {
		prep_compound_gigantic_page(page, huge_page_order(h));
		prep_new_huge_page(h, page, nid);
	}

	return page;
}