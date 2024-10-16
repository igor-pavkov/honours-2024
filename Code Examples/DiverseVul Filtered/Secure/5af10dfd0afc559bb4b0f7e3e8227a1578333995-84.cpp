static void __init report_hugepages(void)
{
	struct hstate *h;

	for_each_hstate(h) {
		char buf[32];

		string_get_size(huge_page_size(h), 1, STRING_UNITS_2, buf, 32);
		pr_info("HugeTLB registered %s page size, pre-allocated %ld pages\n",
			buf, h->free_huge_pages);
	}
}