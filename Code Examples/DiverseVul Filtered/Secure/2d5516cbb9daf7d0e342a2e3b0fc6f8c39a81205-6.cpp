noinline struct pt_regs * __cpuinit __attribute__((weak)) idle_regs(struct pt_regs *regs)
{
	memset(regs, 0, sizeof(struct pt_regs));
	return regs;
}