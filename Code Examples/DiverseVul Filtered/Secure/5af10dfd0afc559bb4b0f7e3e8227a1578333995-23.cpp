static void unmap_ref_private(struct mm_struct *mm, struct vm_area_struct *vma,
			      struct page *page, unsigned long address)
{
	struct hstate *h = hstate_vma(vma);
	struct vm_area_struct *iter_vma;
	struct address_space *mapping;
	pgoff_t pgoff;

	/*
	 * vm_pgoff is in PAGE_SIZE units, hence the different calculation
	 * from page cache lookup which is in HPAGE_SIZE units.
	 */
	address = address & huge_page_mask(h);
	pgoff = ((address - vma->vm_start) >> PAGE_SHIFT) +
			vma->vm_pgoff;
	mapping = vma->vm_file->f_mapping;

	/*
	 * Take the mapping lock for the duration of the table walk. As
	 * this mapping should be shared between all the VMAs,
	 * __unmap_hugepage_range() is called as the lock is already held
	 */
	i_mmap_lock_write(mapping);
	vma_interval_tree_foreach(iter_vma, &mapping->i_mmap, pgoff, pgoff) {
		/* Do not unmap the current VMA */
		if (iter_vma == vma)
			continue;

		/*
		 * Shared VMAs have their own reserves and do not affect
		 * MAP_PRIVATE accounting but it is possible that a shared
		 * VMA is using the same page so check and skip such VMAs.
		 */
		if (iter_vma->vm_flags & VM_MAYSHARE)
			continue;

		/*
		 * Unmap the page from other VMAs without their own reserves.
		 * They get marked to be SIGKILLed if they fault in these
		 * areas. This is because a future no-page fault on this VMA
		 * could insert a zeroed page instead of the data existing
		 * from the time of fork. This would look like data corruption
		 */
		if (!is_vma_resv_set(iter_vma, HPAGE_RESV_OWNER))
			unmap_hugepage_range(iter_vma, address,
					     address + huge_page_size(h), page);
	}
	i_mmap_unlock_write(mapping);
}