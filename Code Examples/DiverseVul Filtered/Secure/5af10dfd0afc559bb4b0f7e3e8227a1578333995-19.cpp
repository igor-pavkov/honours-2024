struct page *alloc_huge_page_node(struct hstate *h, int nid)
{
	gfp_t gfp_mask = htlb_alloc_mask(h);
	struct page *page = NULL;

	if (nid != NUMA_NO_NODE)
		gfp_mask |= __GFP_THISNODE;

	spin_lock(&hugetlb_lock);
	if (h->free_huge_pages - h->resv_huge_pages > 0)
		page = dequeue_huge_page_nodemask(h, gfp_mask, nid, NULL);
	spin_unlock(&hugetlb_lock);

	if (!page)
		page = __alloc_buddy_huge_page(h, gfp_mask, nid, NULL);

	return page;
}