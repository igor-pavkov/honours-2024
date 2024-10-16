static int __init hugetlb_nrpages_setup(char *s)
{
	unsigned long *mhp;
	static unsigned long *last_mhp;

	if (!parsed_valid_hugepagesz) {
		pr_warn("hugepages = %s preceded by "
			"an unsupported hugepagesz, ignoring\n", s);
		parsed_valid_hugepagesz = true;
		return 1;
	}
	/*
	 * !hugetlb_max_hstate means we haven't parsed a hugepagesz= parameter yet,
	 * so this hugepages= parameter goes to the "default hstate".
	 */
	else if (!hugetlb_max_hstate)
		mhp = &default_hstate_max_huge_pages;
	else
		mhp = &parsed_hstate->max_huge_pages;

	if (mhp == last_mhp) {
		pr_warn("hugepages= specified twice without interleaving hugepagesz=, ignoring\n");
		return 1;
	}

	if (sscanf(s, "%lu", mhp) <= 0)
		*mhp = 0;

	/*
	 * Global state is always initialized later in hugetlb_init.
	 * But we need to allocate >= MAX_ORDER hstates here early to still
	 * use the bootmem allocator.
	 */
	if (hugetlb_max_hstate && parsed_hstate->order >= MAX_ORDER)
		hugetlb_hstate_alloc_pages(parsed_hstate);

	last_mhp = mhp;

	return 1;
}