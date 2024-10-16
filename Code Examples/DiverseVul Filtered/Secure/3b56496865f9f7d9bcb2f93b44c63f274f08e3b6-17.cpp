static void bsp_init_amd(struct cpuinfo_x86 *c)
{
	if (cpu_has(c, X86_FEATURE_CONSTANT_TSC)) {

		if (c->x86 > 0x10 ||
		    (c->x86 == 0x10 && c->x86_model >= 0x2)) {
			u64 val;

			rdmsrl(MSR_K7_HWCR, val);
			if (!(val & BIT(24)))
				printk(KERN_WARNING FW_BUG "TSC doesn't count "
					"with P0 frequency!\n");
		}
	}

	if (c->x86 == 0x15) {
		unsigned long upperbit;
		u32 cpuid, assoc;

		cpuid	 = cpuid_edx(0x80000005);
		assoc	 = cpuid >> 16 & 0xff;
		upperbit = ((cpuid >> 24) << 10) / assoc;

		va_align.mask	  = (upperbit - 1) & PAGE_MASK;
		va_align.flags    = ALIGN_VA_32 | ALIGN_VA_64;
	}
}