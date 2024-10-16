void putback_active_hugepage(struct page *page)
{
	VM_BUG_ON_PAGE(!PageHead(page), page);
	spin_lock(&hugetlb_lock);
	set_page_huge_active(page);
	list_move_tail(&page->lru, &(page_hstate(page))->hugepage_activelist);
	spin_unlock(&hugetlb_lock);
	put_page(page);
}