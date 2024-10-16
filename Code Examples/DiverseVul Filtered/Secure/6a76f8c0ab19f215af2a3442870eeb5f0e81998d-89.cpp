static void ftrace_swap_ips(void *a, void *b, int size)
{
	unsigned long *ipa = a;
	unsigned long *ipb = b;
	unsigned long t;

	t = *ipa;
	*ipa = *ipb;
	*ipb = t;
}