static int count_mm_mlocked_page_nr(struct mm_struct *mm,
		unsigned long start, size_t len)
{
	struct vm_area_struct *vma;
	int count = 0;

	if (mm == NULL)
		mm = current->mm;

	vma = find_vma(mm, start);
	if (vma == NULL)
		vma = mm->mmap;

	for (; vma ; vma = vma->vm_next) {
		if (start >= vma->vm_end)
			continue;
		if (start + len <=  vma->vm_start)
			break;
		if (vma->vm_flags & VM_LOCKED) {
			if (start > vma->vm_start)
				count -= (start - vma->vm_start);
			if (start + len < vma->vm_end) {
				count += start + len - vma->vm_start;
				break;
			}
			count += vma->vm_end - vma->vm_start;
		}
	}

	return count >> PAGE_SHIFT;
}