void __set_breakpoint(struct arch_hw_breakpoint *brk)
{
	memcpy(this_cpu_ptr(&current_brk), brk, sizeof(*brk));

	if (dawr_enabled())
		// Power8 or later
		set_dawr(brk);
	else if (!cpu_has_feature(CPU_FTR_ARCH_207S))
		// Power7 or earlier
		set_dabr(brk);
	else
		// Shouldn't happen due to higher level checks
		WARN_ON_ONCE(1);
}