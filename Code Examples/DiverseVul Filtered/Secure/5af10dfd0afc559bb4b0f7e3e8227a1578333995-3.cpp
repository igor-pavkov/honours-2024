static void prep_new_huge_page(struct hstate *h, struct page *page, int nid)
{
	INIT_LIST_HEAD(&page->lru);
	set_compound_page_dtor(page, HUGETLB_PAGE_DTOR);
	spin_lock(&hugetlb_lock);
	set_hugetlb_cgroup(page, NULL);
	h->nr_huge_pages++;
	h->nr_huge_pages_node[nid]++;
	spin_unlock(&hugetlb_lock);
	put_page(page); /* free it into the hugepage allocator */
}