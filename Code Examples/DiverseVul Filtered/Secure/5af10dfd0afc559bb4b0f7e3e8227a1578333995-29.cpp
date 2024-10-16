static struct page *alloc_fresh_huge_page_node(struct hstate *h, int nid)
{
	struct page *page;

	page = __alloc_pages_node(nid,
		htlb_alloc_mask(h)|__GFP_COMP|__GFP_THISNODE|
						__GFP_RETRY_MAYFAIL|__GFP_NOWARN,
		huge_page_order(h));
	if (page) {
		prep_new_huge_page(h, page, nid);
	}

	return page;
}