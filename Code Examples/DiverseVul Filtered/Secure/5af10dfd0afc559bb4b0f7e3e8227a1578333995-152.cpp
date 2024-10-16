static int alloc_fresh_huge_page(struct hstate *h, nodemask_t *nodes_allowed)
{
	struct page *page;
	int nr_nodes, node;
	int ret = 0;

	for_each_node_mask_to_alloc(h, nr_nodes, node, nodes_allowed) {
		page = alloc_fresh_huge_page_node(h, node);
		if (page) {
			ret = 1;
			break;
		}
	}

	if (ret)
		count_vm_event(HTLB_BUDDY_PGALLOC);
	else
		count_vm_event(HTLB_BUDDY_PGALLOC_FAIL);

	return ret;
}