static struct page *__hugetlb_alloc_buddy_huge_page(struct hstate *h,
		gfp_t gfp_mask, int nid, nodemask_t *nmask)
{
	int order = huge_page_order(h);

	gfp_mask |= __GFP_COMP|__GFP_RETRY_MAYFAIL|__GFP_NOWARN;
	if (nid == NUMA_NO_NODE)
		nid = numa_mem_id();
	return __alloc_pages_nodemask(gfp_mask, order, nid, nmask);
}