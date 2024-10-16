static void save_all(struct task_struct *tsk)
{
	unsigned long usermsr;

	if (!tsk->thread.regs)
		return;

	usermsr = tsk->thread.regs->msr;

	if ((usermsr & msr_all_available) == 0)
		return;

	msr_check_and_set(msr_all_available);

	WARN_ON((usermsr & MSR_VSX) && !((usermsr & MSR_FP) && (usermsr & MSR_VEC)));

	if (usermsr & MSR_FP)
		save_fpu(tsk);

	if (usermsr & MSR_VEC)
		save_altivec(tsk);

	if (usermsr & MSR_SPE)
		__giveup_spe(tsk);

	msr_check_and_clear(msr_all_available);
	thread_pkey_regs_save(&tsk->thread);
}