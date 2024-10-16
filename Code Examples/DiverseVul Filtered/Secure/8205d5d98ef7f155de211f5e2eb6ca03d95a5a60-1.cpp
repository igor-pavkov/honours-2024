unsigned long notrace msr_check_and_set(unsigned long bits)
{
	unsigned long oldmsr = mfmsr();
	unsigned long newmsr;

	newmsr = oldmsr | bits;

#ifdef CONFIG_VSX
	if (cpu_has_feature(CPU_FTR_VSX) && (bits & MSR_FP))
		newmsr |= MSR_VSX;
#endif

	if (oldmsr != newmsr)
		mtmsr_isync(newmsr);

	return newmsr;
}