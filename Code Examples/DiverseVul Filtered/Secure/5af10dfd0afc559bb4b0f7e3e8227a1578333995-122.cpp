static void try_to_free_low(struct hstate *h, unsigned long count,
						nodemask_t *nodes_allowed)
{
	int i;

	if (hstate_is_gigantic(h))
		return;

	for_each_node_mask(i, *nodes_allowed) {
		struct page *page, *next;
		struct list_head *freel = &h->hugepage_freelists[i];
		list_for_each_entry_safe(page, next, freel, lru) {
			if (count >= h->nr_huge_pages)
				return;
			if (PageHighMem(page))
				continue;
			list_del(&page->lru);
			update_and_free_page(h, page);
			h->free_huge_pages--;
			h->free_huge_pages_node[page_to_nid(page)]--;
		}
	}
}