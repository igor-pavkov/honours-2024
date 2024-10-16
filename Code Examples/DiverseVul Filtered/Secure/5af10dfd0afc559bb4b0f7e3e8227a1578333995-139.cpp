static int alloc_fresh_gigantic_page(struct hstate *h,
				nodemask_t *nodes_allowed)
{
	struct page *page = NULL;
	int nr_nodes, node;

	for_each_node_mask_to_alloc(h, nr_nodes, node, nodes_allowed) {
		page = alloc_fresh_gigantic_page_node(h, node);
		if (page)
			return 1;
	}

	return 0;
}