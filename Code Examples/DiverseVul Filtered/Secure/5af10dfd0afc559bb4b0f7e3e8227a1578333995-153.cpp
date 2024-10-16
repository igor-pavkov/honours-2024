static int free_pool_huge_page(struct hstate *h, nodemask_t *nodes_allowed,
							 bool acct_surplus)
{
	int nr_nodes, node;
	int ret = 0;

	for_each_node_mask_to_free(h, nr_nodes, node, nodes_allowed) {
		/*
		 * If we're returning unused surplus pages, only examine
		 * nodes with surplus pages.
		 */
		if ((!acct_surplus || h->surplus_huge_pages_node[node]) &&
		    !list_empty(&h->hugepage_freelists[node])) {
			struct page *page =
				list_entry(h->hugepage_freelists[node].next,
					  struct page, lru);
			list_del(&page->lru);
			h->free_huge_pages--;
			h->free_huge_pages_node[node]--;
			if (acct_surplus) {
				h->surplus_huge_pages--;
				h->surplus_huge_pages_node[node]--;
			}
			update_and_free_page(h, page);
			ret = 1;
			break;
		}
	}

	return ret;
}