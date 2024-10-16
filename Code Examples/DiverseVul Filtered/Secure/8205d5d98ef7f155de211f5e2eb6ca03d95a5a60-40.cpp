void flush_all_to_thread(struct task_struct *tsk)
{
	if (tsk->thread.regs) {
		preempt_disable();
		BUG_ON(tsk != current);
#ifdef CONFIG_SPE
		if (tsk->thread.regs->msr & MSR_SPE)
			tsk->thread.spefscr = mfspr(SPRN_SPEFSCR);
#endif
		save_all(tsk);

		preempt_enable();
	}
}