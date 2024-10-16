static inline void tm_recheckpoint_new_task(struct task_struct *new)
{
	if (!cpu_has_feature(CPU_FTR_TM))
		return;

	/* Recheckpoint the registers of the thread we're about to switch to.
	 *
	 * If the task was using FP, we non-lazily reload both the original and
	 * the speculative FP register states.  This is because the kernel
	 * doesn't see if/when a TM rollback occurs, so if we take an FP
	 * unavailable later, we are unable to determine which set of FP regs
	 * need to be restored.
	 */
	if (!tm_enabled(new))
		return;

	if (!MSR_TM_ACTIVE(new->thread.regs->msr)){
		tm_restore_sprs(&new->thread);
		return;
	}
	/* Recheckpoint to restore original checkpointed register state. */
	TM_DEBUG("*** tm_recheckpoint of pid %d (new->msr 0x%lx)\n",
		 new->pid, new->thread.regs->msr);

	tm_recheckpoint(&new->thread);

	/*
	 * The checkpointed state has been restored but the live state has
	 * not, ensure all the math functionality is turned off to trigger
	 * restore_math() to reload.
	 */
	new->thread.regs->msr &= ~(MSR_FP | MSR_VEC | MSR_VSX);

	TM_DEBUG("*** tm_recheckpoint of pid %d complete "
		 "(kernel msr 0x%lx)\n",
		 new->pid, mfmsr());
}