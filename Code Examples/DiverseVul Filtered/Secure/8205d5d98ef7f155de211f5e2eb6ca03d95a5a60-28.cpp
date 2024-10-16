static void __giveup_vsx(struct task_struct *tsk)
{
	unsigned long msr = tsk->thread.regs->msr;

	/*
	 * We should never be ssetting MSR_VSX without also setting
	 * MSR_FP and MSR_VEC
	 */
	WARN_ON((msr & MSR_VSX) && !((msr & MSR_FP) && (msr & MSR_VEC)));

	/* __giveup_fpu will clear MSR_VSX */
	if (msr & MSR_FP)
		__giveup_fpu(tsk);
	if (msr & MSR_VEC)
		__giveup_altivec(tsk);
}