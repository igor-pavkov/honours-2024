SYSCALL_DEFINE0(munlockall)
{
	int ret;

	if (down_write_killable(&current->mm->mmap_sem))
		return -EINTR;
	ret = apply_mlockall_flags(0);
	up_write(&current->mm->mmap_sem);
	return ret;
}