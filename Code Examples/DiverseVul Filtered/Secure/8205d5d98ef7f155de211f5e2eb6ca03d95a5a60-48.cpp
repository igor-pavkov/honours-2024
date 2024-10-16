static void __giveup_fpu(struct task_struct *tsk)
{
	unsigned long msr;

	save_fpu(tsk);
	msr = tsk->thread.regs->msr;
	msr &= ~(MSR_FP|MSR_FE0|MSR_FE1);
#ifdef CONFIG_VSX
	if (cpu_has_feature(CPU_FTR_VSX))
		msr &= ~MSR_VSX;
#endif
	tsk->thread.regs->msr = msr;
}