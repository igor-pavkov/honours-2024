static void __init gather_bootmem_prealloc(void)
{
	struct huge_bootmem_page *m;

	list_for_each_entry(m, &huge_boot_pages, list) {
		struct hstate *h = m->hstate;
		struct page *page;

#ifdef CONFIG_HIGHMEM
		page = pfn_to_page(m->phys >> PAGE_SHIFT);
		memblock_free_late(__pa(m),
				   sizeof(struct huge_bootmem_page));
#else
		page = virt_to_page(m);
#endif
		WARN_ON(page_count(page) != 1);
		prep_compound_huge_page(page, h->order);
		WARN_ON(PageReserved(page));
		prep_new_huge_page(h, page, page_to_nid(page));
		/*
		 * If we had gigantic hugepages allocated at boot time, we need
		 * to restore the 'stolen' pages to totalram_pages in order to
		 * fix confusing memory reports from free(1) and another
		 * side-effects, like CommitLimit going negative.
		 */
		if (hstate_is_gigantic(h))
			adjust_managed_page_count(page, 1 << h->order);
	}
}