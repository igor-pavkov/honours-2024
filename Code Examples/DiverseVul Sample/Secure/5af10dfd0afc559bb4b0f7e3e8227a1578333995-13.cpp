static bool zone_spans_last_pfn(const struct zone *zone,
			unsigned long start_pfn, unsigned long nr_pages)
{
	unsigned long last_pfn = start_pfn + nr_pages - 1;
	return zone_spans_pfn(zone, last_pfn);
}