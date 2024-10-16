void notrace restore_math(struct pt_regs *regs)
{
	unsigned long msr;

	if (!MSR_TM_ACTIVE(regs->msr) &&
		!current->thread.load_fp && !loadvec(current->thread))
		return;

	msr = regs->msr;
	msr_check_and_set(msr_all_available);

	/*
	 * Only reload if the bit is not set in the user MSR, the bit BEING set
	 * indicates that the registers are hot
	 */
	if ((!(msr & MSR_FP)) && restore_fp(current))
		msr |= MSR_FP | current->thread.fpexc_mode;

	if ((!(msr & MSR_VEC)) && restore_altivec(current))
		msr |= MSR_VEC;

	if ((msr & (MSR_FP | MSR_VEC)) == (MSR_FP | MSR_VEC) &&
			restore_vsx(current)) {
		msr |= MSR_VSX;
	}

	msr_check_and_clear(msr_all_available);

	regs->msr = msr;
}