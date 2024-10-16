bool ppc_breakpoint_available(void)
{
	if (dawr_enabled())
		return true; /* POWER8 DAWR or POWER9 forced DAWR */
	if (cpu_has_feature(CPU_FTR_ARCH_207S))
		return false; /* POWER9 with DAWR disabled */
	/* DABR: Everything but POWER8 and POWER9 */
	return true;
}