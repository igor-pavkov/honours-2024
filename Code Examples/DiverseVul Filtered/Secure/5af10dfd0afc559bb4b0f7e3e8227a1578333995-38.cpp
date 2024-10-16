static struct page *dequeue_huge_page_node_exact(struct hstate *h, int nid)
{
	struct page *page;

	list_for_each_entry(page, &h->hugepage_freelists[nid], lru)
		if (!PageHWPoison(page))
			break;
	/*
	 * if 'non-isolated free hugepage' not found on the list,
	 * the allocation fails.
	 */
	if (&h->hugepage_freelists[nid] == &page->lru)
		return NULL;
	list_move(&page->lru, &h->hugepage_activelist);
	set_page_refcounted(page);
	h->free_huge_pages--;
	h->free_huge_pages_node[nid]--;
	return page;
}