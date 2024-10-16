bool page_huge_active(struct page *page)
{
	VM_BUG_ON_PAGE(!PageHuge(page), page);
	return PageHead(page) && PagePrivate(&page[1]);
}