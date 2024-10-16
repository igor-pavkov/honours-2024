static inline void tm_reclaim_task(struct task_struct *tsk)
{
	/* We have to work out if we're switching from/to a task that's in the
	 * middle of a transaction.
	 *
	 * In switching we need to maintain a 2nd register state as
	 * oldtask->thread.ckpt_regs.  We tm_reclaim(oldproc); this saves the
	 * checkpointed (tbegin) state in ckpt_regs, ckfp_state and
	 * ckvr_state
	 *
	 * We also context switch (save) TFHAR/TEXASR/TFIAR in here.
	 */
	struct thread_struct *thr = &tsk->thread;

	if (!thr->regs)
		return;

	if (!MSR_TM_ACTIVE(thr->regs->msr))
		goto out_and_saveregs;

	WARN_ON(tm_suspend_disabled);

	TM_DEBUG("--- tm_reclaim on pid %d (NIP=%lx, "
		 "ccr=%lx, msr=%lx, trap=%lx)\n",
		 tsk->pid, thr->regs->nip,
		 thr->regs->ccr, thr->regs->msr,
		 thr->regs->trap);

	tm_reclaim_thread(thr, TM_CAUSE_RESCHED);

	TM_DEBUG("--- tm_reclaim on pid %d complete\n",
		 tsk->pid);

out_and_saveregs:
	/* Always save the regs here, even if a transaction's not active.
	 * This context-switches a thread's TM info SPRs.  We do it here to
	 * be consistent with the restore path (in recheckpoint) which
	 * cannot happen later in _switch().
	 */
	tm_save_sprs(thr);
}