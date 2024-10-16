static bool hugetlbfs_pagecache_present(struct hstate *h,
			struct vm_area_struct *vma, unsigned long address)
{
	struct address_space *mapping;
	pgoff_t idx;
	struct page *page;

	mapping = vma->vm_file->f_mapping;
	idx = vma_hugecache_offset(h, vma, address);

	page = find_get_page(mapping, idx);
	if (page)
		put_page(page);
	return page != NULL;
}