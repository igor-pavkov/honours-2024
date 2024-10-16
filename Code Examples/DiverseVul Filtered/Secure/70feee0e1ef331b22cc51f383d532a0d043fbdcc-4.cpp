static int apply_vma_lock_flags(unsigned long start, size_t len,
				vm_flags_t flags)
{
	unsigned long nstart, end, tmp;
	struct vm_area_struct * vma, * prev;
	int error;

	VM_BUG_ON(offset_in_page(start));
	VM_BUG_ON(len != PAGE_ALIGN(len));
	end = start + len;
	if (end < start)
		return -EINVAL;
	if (end == start)
		return 0;
	vma = find_vma(current->mm, start);
	if (!vma || vma->vm_start > start)
		return -ENOMEM;

	prev = vma->vm_prev;
	if (start > vma->vm_start)
		prev = vma;

	for (nstart = start ; ; ) {
		vm_flags_t newflags = vma->vm_flags & VM_LOCKED_CLEAR_MASK;

		newflags |= flags;

		/* Here we know that  vma->vm_start <= nstart < vma->vm_end. */
		tmp = vma->vm_end;
		if (tmp > end)
			tmp = end;
		error = mlock_fixup(vma, &prev, nstart, tmp, newflags);
		if (error)
			break;
		nstart = tmp;
		if (nstart < prev->vm_end)
			nstart = prev->vm_end;
		if (nstart >= end)
			break;

		vma = prev->vm_next;
		if (!vma || vma->vm_start != nstart) {
			error = -ENOMEM;
			break;
		}
	}
	return error;
}