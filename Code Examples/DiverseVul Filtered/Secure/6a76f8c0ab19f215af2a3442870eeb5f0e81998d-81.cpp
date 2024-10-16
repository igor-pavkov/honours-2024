static int ftrace_cmp_ips(const void *a, const void *b)
{
	const unsigned long *ipa = a;
	const unsigned long *ipb = b;

	if (*ipa > *ipb)
		return 1;
	if (*ipa < *ipb)
		return -1;
	return 0;
}