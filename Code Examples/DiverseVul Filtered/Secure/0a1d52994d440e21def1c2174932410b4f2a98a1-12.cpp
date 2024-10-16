int vm_munmap(unsigned long start, size_t len)
{
	return __vm_munmap(start, len, false);
}