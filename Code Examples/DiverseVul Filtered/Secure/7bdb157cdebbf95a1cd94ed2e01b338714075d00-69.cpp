static void perf_addr_filter_apply(struct perf_addr_filter *filter,
				   struct mm_struct *mm,
				   struct perf_addr_filter_range *fr)
{
	struct vm_area_struct *vma;

	for (vma = mm->mmap; vma; vma = vma->vm_next) {
		if (!vma->vm_file)
			continue;

		if (perf_addr_filter_vma_adjust(filter, vma, fr))
			return;
	}
}