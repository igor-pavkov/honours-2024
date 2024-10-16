int rosecmp(rose_address *addr1, rose_address *addr2)
{
	int i;

	for (i = 0; i < 5; i++)
		if (addr1->rose_addr[i] != addr2->rose_addr[i])
			return 1;

	return 0;
}