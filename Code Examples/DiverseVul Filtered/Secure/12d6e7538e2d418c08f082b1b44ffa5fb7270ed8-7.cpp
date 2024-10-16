static int hva_to_pfn_slow(unsigned long addr, bool *async, bool write_fault,
			   bool *writable, pfn_t *pfn)
{
	struct page *page[1];
	int npages = 0;

	might_sleep();

	if (writable)
		*writable = write_fault;

	if (async) {
		down_read(&current->mm->mmap_sem);
		npages = get_user_page_nowait(current, current->mm,
					      addr, write_fault, page);
		up_read(&current->mm->mmap_sem);
	} else
		npages = get_user_pages_fast(addr, 1, write_fault,
					     page);
	if (npages != 1)
		return npages;

	/* map read fault as writable if possible */
	if (unlikely(!write_fault) && writable) {
		struct page *wpage[1];

		npages = __get_user_pages_fast(addr, 1, 1, wpage);
		if (npages == 1) {
			*writable = true;
			put_page(page[0]);
			page[0] = wpage[0];
		}

		npages = 1;
	}
	*pfn = page_to_pfn(page[0]);
	return npages;
}