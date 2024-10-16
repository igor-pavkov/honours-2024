static bool perf_addr_filter_vma_adjust(struct perf_addr_filter *filter,
					struct vm_area_struct *vma,
					struct perf_addr_filter_range *fr)
{
	unsigned long vma_size = vma->vm_end - vma->vm_start;
	unsigned long off = vma->vm_pgoff << PAGE_SHIFT;
	struct file *file = vma->vm_file;

	if (!perf_addr_filter_match(filter, file, off, vma_size))
		return false;

	if (filter->offset < off) {
		fr->start = vma->vm_start;
		fr->size = min(vma_size, filter->size - (off - filter->offset));
	} else {
		fr->start = vma->vm_start + filter->offset - off;
		fr->size = min(vma->vm_end - fr->start, filter->size);
	}

	return true;
}