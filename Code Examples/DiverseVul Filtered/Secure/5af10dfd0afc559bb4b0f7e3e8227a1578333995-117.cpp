static void set_page_huge_active(struct page *page)
{
	VM_BUG_ON_PAGE(!PageHeadHuge(page), page);
	SetPagePrivate(&page[1]);
}