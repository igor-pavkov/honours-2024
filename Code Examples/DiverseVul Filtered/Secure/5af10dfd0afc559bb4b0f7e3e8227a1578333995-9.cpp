struct page *alloc_huge_page_nodemask(struct hstate *h, int preferred_nid,
		nodemask_t *nmask)
{
	gfp_t gfp_mask = htlb_alloc_mask(h);

	spin_lock(&hugetlb_lock);
	if (h->free_huge_pages - h->resv_huge_pages > 0) {
		struct page *page;

		page = dequeue_huge_page_nodemask(h, gfp_mask, preferred_nid, nmask);
		if (page) {
			spin_unlock(&hugetlb_lock);
			return page;
		}
	}
	spin_unlock(&hugetlb_lock);

	/* No reservations, try to overcommit */

	return __alloc_buddy_huge_page(h, gfp_mask, preferred_nid, nmask);
}