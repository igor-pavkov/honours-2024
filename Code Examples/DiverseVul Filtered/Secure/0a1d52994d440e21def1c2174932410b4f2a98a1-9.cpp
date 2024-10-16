int do_munmap(struct mm_struct *mm, unsigned long start, size_t len,
	      struct list_head *uf)
{
	return __do_munmap(mm, start, len, uf, false);
}