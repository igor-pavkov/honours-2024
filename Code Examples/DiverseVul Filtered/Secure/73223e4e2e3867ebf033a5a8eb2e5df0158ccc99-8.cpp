static struct page *alloc_page_interleave(gfp_t gfp, unsigned order,
					unsigned nid)
{
	struct page *page;

	page = __alloc_pages(gfp, order, nid);
	if (page && page_to_nid(page) == nid)
		inc_zone_page_state(page, NUMA_INTERLEAVE_HIT);
	return page;
}