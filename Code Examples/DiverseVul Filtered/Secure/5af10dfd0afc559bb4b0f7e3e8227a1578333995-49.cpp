static void clear_page_huge_active(struct page *page)
{
	VM_BUG_ON_PAGE(!PageHeadHuge(page), page);
	ClearPagePrivate(&page[1]);
}