struct page *__page_cache_alloc(gfp_t gfp)
{
	if (cpuset_do_page_mem_spread()) {
		int n = cpuset_mem_spread_node();
		return alloc_pages_node(n, gfp, 0);
	}
	return alloc_pages(gfp, 0);
}