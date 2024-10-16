static inline int use_zero_page(struct vm_area_struct *vma)
{
	/*
	 * We don't want to optimize FOLL_ANON for make_pages_present()
	 * when it tries to page in a VM_LOCKED region. As to VM_SHARED,
	 * we want to get the page from the page tables to make sure
	 * that we serialize and update with any other user of that
	 * mapping.
	 */
	if (vma->vm_flags & (VM_LOCKED | VM_SHARED))
		return 0;
	/*
	 * And if we have a fault or a nopfn routine, it's not an
	 * anonymous region.
	 */
	return !vma->vm_ops ||
		(!vma->vm_ops->fault && !vma->vm_ops->nopfn);
}