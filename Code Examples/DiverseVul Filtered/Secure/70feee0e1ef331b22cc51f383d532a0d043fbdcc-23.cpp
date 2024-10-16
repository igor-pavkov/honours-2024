static int apply_mlockall_flags(int flags)
{
	struct vm_area_struct * vma, * prev = NULL;
	vm_flags_t to_add = 0;

	current->mm->def_flags &= VM_LOCKED_CLEAR_MASK;
	if (flags & MCL_FUTURE) {
		current->mm->def_flags |= VM_LOCKED;

		if (flags & MCL_ONFAULT)
			current->mm->def_flags |= VM_LOCKONFAULT;

		if (!(flags & MCL_CURRENT))
			goto out;
	}

	if (flags & MCL_CURRENT) {
		to_add |= VM_LOCKED;
		if (flags & MCL_ONFAULT)
			to_add |= VM_LOCKONFAULT;
	}

	for (vma = current->mm->mmap; vma ; vma = prev->vm_next) {
		vm_flags_t newflags;

		newflags = vma->vm_flags & VM_LOCKED_CLEAR_MASK;
		newflags |= to_add;

		/* Ignore errors */
		mlock_fixup(vma, &prev, vma->vm_start, vma->vm_end, newflags);
		cond_resched_rcu_qs();
	}
out:
	return 0;
}