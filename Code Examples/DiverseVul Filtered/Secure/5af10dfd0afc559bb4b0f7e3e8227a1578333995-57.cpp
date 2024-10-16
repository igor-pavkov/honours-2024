static void free_gigantic_page(struct page *page, unsigned int order)
{
	free_contig_range(page_to_pfn(page), 1 << order);
}