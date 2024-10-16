int lxc_strmunmap(void *addr, size_t length)
{
	return munmap(addr, length + 1);
}