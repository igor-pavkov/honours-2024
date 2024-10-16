unsigned long hugetlb_total_pages(void)
{
	struct hstate *h;
	unsigned long nr_total_pages = 0;

	for_each_hstate(h)
		nr_total_pages += h->nr_huge_pages * pages_per_huge_page(h);
	return nr_total_pages;
}