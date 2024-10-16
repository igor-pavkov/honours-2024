static int adjust_pool_surplus(struct hstate *h, nodemask_t *nodes_allowed,
				int delta)
{
	int nr_nodes, node;

	VM_BUG_ON(delta != -1 && delta != 1);

	if (delta < 0) {
		for_each_node_mask_to_alloc(h, nr_nodes, node, nodes_allowed) {
			if (h->surplus_huge_pages_node[node])
				goto found;
		}
	} else {
		for_each_node_mask_to_free(h, nr_nodes, node, nodes_allowed) {
			if (h->surplus_huge_pages_node[node] <
					h->nr_huge_pages_node[node])
				goto found;
		}
	}
	return 0;

found:
	h->surplus_huge_pages += delta;
	h->surplus_huge_pages_node[node] += delta;
	return 1;
}