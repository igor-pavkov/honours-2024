void __pte_error(const char *file, int line, unsigned long val)
{
	printk("%s:%d: bad pte %016lx.\n", file, line, val);
}