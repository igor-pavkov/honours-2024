static void print_ip_ins(const char *fmt, unsigned char *p)
{
	int i;

	printk(KERN_CONT "%s", fmt);

	for (i = 0; i < MCOUNT_INSN_SIZE; i++)
		printk(KERN_CONT "%s%02x", i ? ":" : "", p[i]);
}