SYSCALL_DEFINE2(munlock, unsigned long, start, size_t, len)
{
	int ret;

	len = PAGE_ALIGN(len + (offset_in_page(start)));
	start &= PAGE_MASK;

	if (down_write_killable(&current->mm->mmap_sem))
		return -EINTR;
	ret = apply_vma_lock_flags(start, len, 0);
	up_write(&current->mm->mmap_sem);

	return ret;
}