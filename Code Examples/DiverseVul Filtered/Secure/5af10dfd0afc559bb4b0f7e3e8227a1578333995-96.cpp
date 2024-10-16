int PageHeadHuge(struct page *page_head)
{
	if (!PageHead(page_head))
		return 0;

	return get_compound_page_dtor(page_head) == free_huge_page;
}