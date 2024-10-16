uint64_t lxc_find_next_power2(uint64_t n)
{
	/* 0 is not valid input. We return 0 to the caller since 0 is not a
	 * valid power of two.
	 */
	if (n == 0)
		return 0;

	if (!(n & (n - 1)))
		return n;

	while (n & (n - 1))
		n = n & (n - 1);

	n = n << 1;
	return n;
}