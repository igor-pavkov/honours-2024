static int selinux_file_mprotect(struct vm_area_struct *vma,
				 unsigned long reqprot,
				 unsigned long prot)
{
	const struct cred *cred = current_cred();

	if (selinux_checkreqprot)
		prot = reqprot;

	if (default_noexec &&
	    (prot & PROT_EXEC) && !(vma->vm_flags & VM_EXEC)) {
		int rc = 0;
		if (vma->vm_start >= vma->vm_mm->start_brk &&
		    vma->vm_end <= vma->vm_mm->brk) {
			rc = cred_has_perm(cred, cred, PROCESS__EXECHEAP);
		} else if (!vma->vm_file &&
			   ((vma->vm_start <= vma->vm_mm->start_stack &&
			     vma->vm_end >= vma->vm_mm->start_stack) ||
			    vma_is_stack_for_current(vma))) {
			rc = current_has_perm(current, PROCESS__EXECSTACK);
		} else if (vma->vm_file && vma->anon_vma) {
			/*
			 * We are making executable a file mapping that has
			 * had some COW done. Since pages might have been
			 * written, check ability to execute the possibly
			 * modified content.  This typically should only
			 * occur for text relocations.
			 */
			rc = file_has_perm(cred, vma->vm_file, FILE__EXECMOD);
		}
		if (rc)
			return rc;
	}

	return file_map_prot_check(vma->vm_file, prot, vma->vm_flags&VM_SHARED);
}