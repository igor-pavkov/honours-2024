void restore_tm_state(struct pt_regs *regs)
{
	unsigned long msr_diff;

	/*
	 * This is the only moment we should clear TIF_RESTORE_TM as
	 * it is here that ckpt_regs.msr and pt_regs.msr become the same
	 * again, anything else could lead to an incorrect ckpt_msr being
	 * saved and therefore incorrect signal contexts.
	 */
	clear_thread_flag(TIF_RESTORE_TM);
	if (!MSR_TM_ACTIVE(regs->msr))
		return;

	msr_diff = current->thread.ckpt_regs.msr & ~regs->msr;
	msr_diff &= MSR_FP | MSR_VEC | MSR_VSX;

	/* Ensure that restore_math() will restore */
	if (msr_diff & MSR_FP)
		current->thread.load_fp = 1;
#ifdef CONFIG_ALTIVEC
	if (cpu_has_feature(CPU_FTR_ALTIVEC) && msr_diff & MSR_VEC)
		current->thread.load_vec = 1;
#endif
	restore_math(regs);

	regs->msr |= msr_diff;
}