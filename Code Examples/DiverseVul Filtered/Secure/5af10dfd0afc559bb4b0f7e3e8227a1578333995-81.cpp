static int __init hugetlb_init(void)
{
	int i;

	if (!hugepages_supported())
		return 0;

	if (!size_to_hstate(default_hstate_size)) {
		if (default_hstate_size != 0) {
			pr_err("HugeTLB: unsupported default_hugepagesz %lu. Reverting to %lu\n",
			       default_hstate_size, HPAGE_SIZE);
		}

		default_hstate_size = HPAGE_SIZE;
		if (!size_to_hstate(default_hstate_size))
			hugetlb_add_hstate(HUGETLB_PAGE_ORDER);
	}
	default_hstate_idx = hstate_index(size_to_hstate(default_hstate_size));
	if (default_hstate_max_huge_pages) {
		if (!default_hstate.max_huge_pages)
			default_hstate.max_huge_pages = default_hstate_max_huge_pages;
	}

	hugetlb_init_hstates();
	gather_bootmem_prealloc();
	report_hugepages();

	hugetlb_sysfs_init();
	hugetlb_register_all_nodes();
	hugetlb_cgroup_file_init();

#ifdef CONFIG_SMP
	num_fault_mutexes = roundup_pow_of_two(8 * num_possible_cpus());
#else
	num_fault_mutexes = 1;
#endif
	hugetlb_fault_mutex_table =
		kmalloc(sizeof(struct mutex) * num_fault_mutexes, GFP_KERNEL);
	BUG_ON(!hugetlb_fault_mutex_table);

	for (i = 0; i < num_fault_mutexes; i++)
		mutex_init(&hugetlb_fault_mutex_table[i]);
	return 0;
}