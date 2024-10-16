static void init_amd_k7(struct cpuinfo_x86 *c)
{
	u32 l, h;

	/*
	 * Bit 15 of Athlon specific MSR 15, needs to be 0
	 * to enable SSE on Palomino/Morgan/Barton CPU's.
	 * If the BIOS didn't enable it already, enable it here.
	 */
	if (c->x86_model >= 6 && c->x86_model <= 10) {
		if (!cpu_has(c, X86_FEATURE_XMM)) {
			printk(KERN_INFO "Enabling disabled K7/SSE Support.\n");
			rdmsr(MSR_K7_HWCR, l, h);
			l &= ~0x00008000;
			wrmsr(MSR_K7_HWCR, l, h);
			set_cpu_cap(c, X86_FEATURE_XMM);
		}
	}

	/*
	 * It's been determined by AMD that Athlons since model 8 stepping 1
	 * are more robust with CLK_CTL set to 200xxxxx instead of 600xxxxx
	 * As per AMD technical note 27212 0.2
	 */
	if ((c->x86_model == 8 && c->x86_mask >= 1) || (c->x86_model > 8)) {
		rdmsr(MSR_K7_CLK_CTL, l, h);
		if ((l & 0xfff00000) != 0x20000000) {
			printk(KERN_INFO
			    "CPU: CLK_CTL MSR was %x. Reprogramming to %x\n",
					l, ((l & 0x000fffff)|0x20000000));
			wrmsr(MSR_K7_CLK_CTL, (l & 0x000fffff)|0x20000000, h);
		}
	}

	set_cpu_cap(c, X86_FEATURE_K7);

	amd_k7_smp_check(c);
}