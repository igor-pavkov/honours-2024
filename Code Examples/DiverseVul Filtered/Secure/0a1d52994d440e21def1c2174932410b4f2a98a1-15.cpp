int vm_brk_flags(unsigned long addr, unsigned long request, unsigned long flags)
{
	struct mm_struct *mm = current->mm;
	unsigned long len;
	int ret;
	bool populate;
	LIST_HEAD(uf);

	len = PAGE_ALIGN(request);
	if (len < request)
		return -ENOMEM;
	if (!len)
		return 0;

	if (down_write_killable(&mm->mmap_sem))
		return -EINTR;

	ret = do_brk_flags(addr, len, flags, &uf);
	populate = ((mm->def_flags & VM_LOCKED) != 0);
	up_write(&mm->mmap_sem);
	userfaultfd_unmap_complete(mm, &uf);
	if (populate && !ret)
		mm_populate(addr, len);
	return ret;
}