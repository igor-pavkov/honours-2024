bool isolate_huge_page(struct page *page, struct list_head *list)
{
	bool ret = true;

	VM_BUG_ON_PAGE(!PageHead(page), page);
	spin_lock(&hugetlb_lock);
	if (!page_huge_active(page) || !get_page_unless_zero(page)) {
		ret = false;
		goto unlock;
	}
	clear_page_huge_active(page);
	list_move_tail(&page->lru, list);
unlock:
	spin_unlock(&hugetlb_lock);
	return ret;
}