struct page *alloc_huge_page_noerr(struct vm_area_struct *vma,
				unsigned long addr, int avoid_reserve)
{
	struct page *page = alloc_huge_page(vma, addr, avoid_reserve);
	if (IS_ERR(page))
		page = NULL;
	return page;
}