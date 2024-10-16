static struct page *alloc_gigantic_page(int nid, unsigned int order)
{
	unsigned long nr_pages = 1 << order;
	unsigned long ret, pfn, flags;
	struct zone *z;

	z = NODE_DATA(nid)->node_zones;
	for (; z - NODE_DATA(nid)->node_zones < MAX_NR_ZONES; z++) {
		spin_lock_irqsave(&z->lock, flags);

		pfn = ALIGN(z->zone_start_pfn, nr_pages);
		while (zone_spans_last_pfn(z, pfn, nr_pages)) {
			if (pfn_range_valid_gigantic(z, pfn, nr_pages)) {
				/*
				 * We release the zone lock here because
				 * alloc_contig_range() will also lock the zone
				 * at some point. If there's an allocation
				 * spinning on this lock, it may win the race
				 * and cause alloc_contig_range() to fail...
				 */
				spin_unlock_irqrestore(&z->lock, flags);
				ret = __alloc_gigantic_page(pfn, nr_pages);
				if (!ret)
					return pfn_to_page(pfn);
				spin_lock_irqsave(&z->lock, flags);
			}
			pfn += nr_pages;
		}

		spin_unlock_irqrestore(&z->lock, flags);
	}

	return NULL;
}