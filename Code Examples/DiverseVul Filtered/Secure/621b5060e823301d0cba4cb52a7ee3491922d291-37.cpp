static inline int set_dabr(struct arch_hw_breakpoint *brk)
{
	unsigned long dabr, dabrx;

	dabr = brk->address | (brk->type & HW_BRK_TYPE_DABR);
	dabrx = ((brk->type >> 3) & 0x7);

	if (ppc_md.set_dabr)
		return ppc_md.set_dabr(dabr, dabrx);

	return __set_dabr(dabr, dabrx);
}