static int __vm_munmap(unsigned long start, size_t len, bool downgrade)
{
	int ret;
	struct mm_struct *mm = current->mm;
	LIST_HEAD(uf);

	if (down_write_killable(&mm->mmap_sem))
		return -EINTR;

	ret = __do_munmap(mm, start, len, &uf, downgrade);
	/*
	 * Returning 1 indicates mmap_sem is downgraded.
	 * But 1 is not legal return value of vm_munmap() and munmap(), reset
	 * it to 0 before return.
	 */
	if (ret == 1) {
		up_read(&mm->mmap_sem);
		ret = 0;
	} else
		up_write(&mm->mmap_sem);

	userfaultfd_unmap_complete(mm, &uf);
	return ret;
}