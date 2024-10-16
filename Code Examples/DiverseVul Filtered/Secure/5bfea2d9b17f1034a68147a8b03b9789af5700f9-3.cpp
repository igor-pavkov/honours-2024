SYSCALL_DEFINE5(mremap, unsigned long, addr, unsigned long, old_len,
		unsigned long, new_len, unsigned long, flags,
		unsigned long, new_addr)
{
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;
	unsigned long ret = -EINVAL;
	unsigned long charged = 0;
	bool locked = false;
	bool downgraded = false;
	struct vm_userfaultfd_ctx uf = NULL_VM_UFFD_CTX;
	LIST_HEAD(uf_unmap_early);
	LIST_HEAD(uf_unmap);

	/*
	 * There is a deliberate asymmetry here: we strip the pointer tag
	 * from the old address but leave the new address alone. This is
	 * for consistency with mmap(), where we prevent the creation of
	 * aliasing mappings in userspace by leaving the tag bits of the
	 * mapping address intact. A non-zero tag will cause the subsequent
	 * range checks to reject the address as invalid.
	 *
	 * See Documentation/arm64/tagged-address-abi.rst for more information.
	 */
	addr = untagged_addr(addr);

	if (flags & ~(MREMAP_FIXED | MREMAP_MAYMOVE | MREMAP_DONTUNMAP))
		return ret;

	if (flags & MREMAP_FIXED && !(flags & MREMAP_MAYMOVE))
		return ret;

	/*
	 * MREMAP_DONTUNMAP is always a move and it does not allow resizing
	 * in the process.
	 */
	if (flags & MREMAP_DONTUNMAP &&
			(!(flags & MREMAP_MAYMOVE) || old_len != new_len))
		return ret;


	if (offset_in_page(addr))
		return ret;

	old_len = PAGE_ALIGN(old_len);
	new_len = PAGE_ALIGN(new_len);

	/*
	 * We allow a zero old-len as a special case
	 * for DOS-emu "duplicate shm area" thing. But
	 * a zero new-len is nonsensical.
	 */
	if (!new_len)
		return ret;

	if (down_write_killable(&current->mm->mmap_sem))
		return -EINTR;

	if (flags & (MREMAP_FIXED | MREMAP_DONTUNMAP)) {
		ret = mremap_to(addr, old_len, new_addr, new_len,
				&locked, flags, &uf, &uf_unmap_early,
				&uf_unmap);
		goto out;
	}

	/*
	 * Always allow a shrinking remap: that just unmaps
	 * the unnecessary pages..
	 * __do_munmap does all the needed commit accounting, and
	 * downgrades mmap_sem to read if so directed.
	 */
	if (old_len >= new_len) {
		int retval;

		retval = __do_munmap(mm, addr+new_len, old_len - new_len,
				  &uf_unmap, true);
		if (retval < 0 && old_len != new_len) {
			ret = retval;
			goto out;
		/* Returning 1 indicates mmap_sem is downgraded to read. */
		} else if (retval == 1)
			downgraded = true;
		ret = addr;
		goto out;
	}

	/*
	 * Ok, we need to grow..
	 */
	vma = vma_to_resize(addr, old_len, new_len, flags, &charged);
	if (IS_ERR(vma)) {
		ret = PTR_ERR(vma);
		goto out;
	}

	/* old_len exactly to the end of the area..
	 */
	if (old_len == vma->vm_end - addr) {
		/* can we just expand the current mapping? */
		if (vma_expandable(vma, new_len - old_len)) {
			int pages = (new_len - old_len) >> PAGE_SHIFT;

			if (vma_adjust(vma, vma->vm_start, addr + new_len,
				       vma->vm_pgoff, NULL)) {
				ret = -ENOMEM;
				goto out;
			}

			vm_stat_account(mm, vma->vm_flags, pages);
			if (vma->vm_flags & VM_LOCKED) {
				mm->locked_vm += pages;
				locked = true;
				new_addr = addr;
			}
			ret = addr;
			goto out;
		}
	}

	/*
	 * We weren't able to just expand or shrink the area,
	 * we need to create a new one and move it..
	 */
	ret = -ENOMEM;
	if (flags & MREMAP_MAYMOVE) {
		unsigned long map_flags = 0;
		if (vma->vm_flags & VM_MAYSHARE)
			map_flags |= MAP_SHARED;

		new_addr = get_unmapped_area(vma->vm_file, 0, new_len,
					vma->vm_pgoff +
					((addr - vma->vm_start) >> PAGE_SHIFT),
					map_flags);
		if (IS_ERR_VALUE(new_addr)) {
			ret = new_addr;
			goto out;
		}

		ret = move_vma(vma, addr, old_len, new_len, new_addr,
			       &locked, flags, &uf, &uf_unmap);
	}
out:
	if (offset_in_page(ret)) {
		vm_unacct_memory(charged);
		locked = 0;
	}
	if (downgraded)
		up_read(&current->mm->mmap_sem);
	else
		up_write(&current->mm->mmap_sem);
	if (locked && new_len > old_len)
		mm_populate(new_addr + old_len, new_len - old_len);
	userfaultfd_unmap_complete(mm, &uf_unmap_early);
	mremap_userfaultfd_complete(&uf, addr, ret, old_len);
	userfaultfd_unmap_complete(mm, &uf_unmap);
	return ret;
}