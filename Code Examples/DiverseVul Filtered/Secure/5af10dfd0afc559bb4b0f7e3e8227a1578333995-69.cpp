void __init hugetlb_add_hstate(unsigned int order)
{
	struct hstate *h;
	unsigned long i;

	if (size_to_hstate(PAGE_SIZE << order)) {
		pr_warn("hugepagesz= specified twice, ignoring\n");
		return;
	}
	BUG_ON(hugetlb_max_hstate >= HUGE_MAX_HSTATE);
	BUG_ON(order == 0);
	h = &hstates[hugetlb_max_hstate++];
	h->order = order;
	h->mask = ~((1ULL << (order + PAGE_SHIFT)) - 1);
	h->nr_huge_pages = 0;
	h->free_huge_pages = 0;
	for (i = 0; i < MAX_NUMNODES; ++i)
		INIT_LIST_HEAD(&h->hugepage_freelists[i]);
	INIT_LIST_HEAD(&h->hugepage_activelist);
	h->next_nid_to_alloc = first_memory_node;
	h->next_nid_to_free = first_memory_node;
	snprintf(h->name, HSTATE_NAME_LEN, "hugepages-%lukB",
					huge_page_size(h)/1024);

	parsed_hstate = h;
}