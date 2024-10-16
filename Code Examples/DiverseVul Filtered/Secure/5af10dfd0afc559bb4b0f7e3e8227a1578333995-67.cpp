void hugetlb_report_meminfo(struct seq_file *m)
{
	struct hstate *h = &default_hstate;
	if (!hugepages_supported())
		return;
	seq_printf(m,
			"HugePages_Total:   %5lu\n"
			"HugePages_Free:    %5lu\n"
			"HugePages_Rsvd:    %5lu\n"
			"HugePages_Surp:    %5lu\n"
			"Hugepagesize:   %8lu kB\n",
			h->nr_huge_pages,
			h->free_huge_pages,
			h->resv_huge_pages,
			h->surplus_huge_pages,
			1UL << (huge_page_order(h) + PAGE_SHIFT - 10));
}