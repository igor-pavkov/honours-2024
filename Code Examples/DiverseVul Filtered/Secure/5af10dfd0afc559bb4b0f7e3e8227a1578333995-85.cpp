static void __init hugetlb_init_hstates(void)
{
	struct hstate *h;

	for_each_hstate(h) {
		if (minimum_order > huge_page_order(h))
			minimum_order = huge_page_order(h);

		/* oversize hugepages were init'ed in early boot */
		if (!hstate_is_gigantic(h))
			hugetlb_hstate_alloc_pages(h);
	}
	VM_BUG_ON(minimum_order == UINT_MAX);
}