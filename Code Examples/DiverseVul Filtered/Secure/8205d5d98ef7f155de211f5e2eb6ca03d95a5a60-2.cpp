void enable_kernel_fp(void)
{
	unsigned long cpumsr;

	WARN_ON(preemptible());

	cpumsr = msr_check_and_set(MSR_FP);

	if (current->thread.regs && (current->thread.regs->msr & MSR_FP)) {
		check_if_tm_restore_required(current);
		/*
		 * If a thread has already been reclaimed then the
		 * checkpointed registers are on the CPU but have definitely
		 * been saved by the reclaim code. Don't need to and *cannot*
		 * giveup as this would save  to the 'live' structure not the
		 * checkpointed structure.
		 */
		if (!MSR_TM_ACTIVE(cpumsr) &&
		     MSR_TM_ACTIVE(current->thread.regs->msr))
			return;
		__giveup_fpu(current);
	}
}