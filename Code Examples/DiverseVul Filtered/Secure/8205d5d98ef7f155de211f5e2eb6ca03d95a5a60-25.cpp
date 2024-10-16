void flush_vsx_to_thread(struct task_struct *tsk)
{
	if (tsk->thread.regs) {
		preempt_disable();
		if (tsk->thread.regs->msr & (MSR_VSX|MSR_VEC|MSR_FP)) {
			BUG_ON(tsk != current);
			giveup_vsx(tsk);
		}
		preempt_enable();
	}
}