void *lxc_strmmap(void *addr, size_t length, int prot, int flags, int fd,
		  off_t offset)
{
	void *tmp = NULL, *overlap = NULL;

	/* We establish an anonymous mapping that is one byte larger than the
	 * underlying file. The pages handed to us are zero filled. */
	tmp = mmap(addr, length + 1, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (tmp == MAP_FAILED)
		return tmp;

	/* Now we establish a fixed-address mapping starting at the address we
	 * received from our anonymous mapping and replace all bytes excluding
	 * the additional \0-byte with the file. This allows us to use normal
	 * string-handling functions. */
	overlap = mmap(tmp, length, prot, MAP_FIXED | flags, fd, offset);
	if (overlap == MAP_FAILED)
		munmap(tmp, length + 1);

	return overlap;
}