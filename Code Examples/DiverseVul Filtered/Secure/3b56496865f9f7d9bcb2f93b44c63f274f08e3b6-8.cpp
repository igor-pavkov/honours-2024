static void cpu_set_tlb_flushall_shift(struct cpuinfo_x86 *c)
{
	tlb_flushall_shift = 5;

	if (c->x86 <= 0x11)
		tlb_flushall_shift = 4;
}