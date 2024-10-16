static void hugetlb_unregister_node(struct node *node)
{
	struct hstate *h;
	struct node_hstate *nhs = &node_hstates[node->dev.id];

	if (!nhs->hugepages_kobj)
		return;		/* no hstate attributes */

	for_each_hstate(h) {
		int idx = hstate_index(h);
		if (nhs->hstate_kobjs[idx]) {
			kobject_put(nhs->hstate_kobjs[idx]);
			nhs->hstate_kobjs[idx] = NULL;
		}
	}

	kobject_put(nhs->hugepages_kobj);
	nhs->hugepages_kobj = NULL;
}