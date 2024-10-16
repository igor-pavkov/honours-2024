static struct vm_area_struct *vma_to_resize(unsigned long addr,
	unsigned long old_len, unsigned long new_len, unsigned long flags,
	unsigned long *p)
{
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma = find_vma(mm, addr);
	unsigned long pgoff;

	if (!vma || vma->vm_start > addr)
		return ERR_PTR(-EFAULT);

	/*
	 * !old_len is a special case where an attempt is made to 'duplicate'
	 * a mapping.  This makes no sense for private mappings as it will
	 * instead create a fresh/new mapping unrelated to the original.  This
	 * is contrary to the basic idea of mremap which creates new mappings
	 * based on the original.  There are no known use cases for this
	 * behavior.  As a result, fail such attempts.
	 */
	if (!old_len && !(vma->vm_flags & (VM_SHARED | VM_MAYSHARE))) {
		pr_warn_once("%s (%d): attempted to duplicate a private mapping with mremap.  This is not supported.\n", current->comm, current->pid);
		return ERR_PTR(-EINVAL);
	}

	if (flags & MREMAP_DONTUNMAP && (!vma_is_anonymous(vma) ||
			vma->vm_flags & VM_SHARED))
		return ERR_PTR(-EINVAL);

	if (is_vm_hugetlb_page(vma))
		return ERR_PTR(-EINVAL);

	/* We can't remap across vm area boundaries */
	if (old_len > vma->vm_end - addr)
		return ERR_PTR(-EFAULT);

	if (new_len == old_len)
		return vma;

	/* Need to be careful about a growing mapping */
	pgoff = (addr - vma->vm_start) >> PAGE_SHIFT;
	pgoff += vma->vm_pgoff;
	if (pgoff + (new_len >> PAGE_SHIFT) < pgoff)
		return ERR_PTR(-EINVAL);

	if (vma->vm_flags & (VM_DONTEXPAND | VM_PFNMAP))
		return ERR_PTR(-EFAULT);

	if (vma->vm_flags & VM_LOCKED) {
		unsigned long locked, lock_limit;
		locked = mm->locked_vm << PAGE_SHIFT;
		lock_limit = rlimit(RLIMIT_MEMLOCK);
		locked += new_len - old_len;
		if (locked > lock_limit && !capable(CAP_IPC_LOCK))
			return ERR_PTR(-EAGAIN);
	}

	if (!may_expand_vm(mm, vma->vm_flags,
				(new_len - old_len) >> PAGE_SHIFT))
		return ERR_PTR(-ENOMEM);

	if (vma->vm_flags & VM_ACCOUNT) {
		unsigned long charged = (new_len - old_len) >> PAGE_SHIFT;
		if (security_vm_enough_memory_mm(mm, charged))
			return ERR_PTR(-ENOMEM);
		*p = charged;
	}

	return vma;
}