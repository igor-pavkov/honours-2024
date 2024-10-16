static bool pfn_range_valid_gigantic(struct zone *z,
			unsigned long start_pfn, unsigned long nr_pages)
{
	unsigned long i, end_pfn = start_pfn + nr_pages;
	struct page *page;

	for (i = start_pfn; i < end_pfn; i++) {
		if (!pfn_valid(i))
			return false;

		page = pfn_to_page(i);

		if (page_zone(page) != z)
			return false;

		if (PageReserved(page))
			return false;

		if (page_count(page) > 0)
			return false;

		if (PageHuge(page))
			return false;
	}

	return true;
}