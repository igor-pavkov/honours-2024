static void restore_reserve_on_error(struct hstate *h,
			struct vm_area_struct *vma, unsigned long address,
			struct page *page)
{
	if (unlikely(PagePrivate(page))) {
		long rc = vma_needs_reservation(h, vma, address);

		if (unlikely(rc < 0)) {
			/*
			 * Rare out of memory condition in reserve map
			 * manipulation.  Clear PagePrivate so that
			 * global reserve count will not be incremented
			 * by free_huge_page.  This will make it appear
			 * as though the reservation for this page was
			 * consumed.  This may prevent the task from
			 * faulting in the page at a later time.  This
			 * is better than inconsistent global huge page
			 * accounting of reserve counts.
			 */
			ClearPagePrivate(page);
		} else if (rc) {
			rc = vma_add_reservation(h, vma, address);
			if (unlikely(rc < 0))
				/*
				 * See above comment about rare out of
				 * memory condition.
				 */
				ClearPagePrivate(page);
		} else
			vma_end_reservation(h, vma, address);
	}
}