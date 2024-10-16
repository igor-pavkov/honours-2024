static long madvise_remove(struct vm_area_struct *vma,
				struct vm_area_struct **prev,
				unsigned long start, unsigned long end)
{
	loff_t offset;
	int error;

	*prev = NULL;	/* tell sys_madvise we drop mmap_sem */

	if (vma->vm_flags & (VM_LOCKED|VM_NONLINEAR|VM_HUGETLB))
		return -EINVAL;

	if (!vma->vm_file || !vma->vm_file->f_mapping
		|| !vma->vm_file->f_mapping->host) {
			return -EINVAL;
	}

	if ((vma->vm_flags & (VM_SHARED|VM_WRITE)) != (VM_SHARED|VM_WRITE))
		return -EACCES;

	offset = (loff_t)(start - vma->vm_start)
			+ ((loff_t)vma->vm_pgoff << PAGE_SHIFT);

	/* filesystem's fallocate may need to take i_mutex */
	up_read(&current->mm->mmap_sem);
	error = do_fallocate(vma->vm_file,
				FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE,
				offset, end - start);
	down_read(&current->mm->mmap_sem);
	return error;
}