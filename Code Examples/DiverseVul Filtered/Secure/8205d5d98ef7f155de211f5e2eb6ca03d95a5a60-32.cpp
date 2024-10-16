static void tm_reclaim_thread(struct thread_struct *thr, uint8_t cause)
{
	/*
	 * Use the current MSR TM suspended bit to track if we have
	 * checkpointed state outstanding.
	 * On signal delivery, we'd normally reclaim the checkpointed
	 * state to obtain stack pointer (see:get_tm_stackpointer()).
	 * This will then directly return to userspace without going
	 * through __switch_to(). However, if the stack frame is bad,
	 * we need to exit this thread which calls __switch_to() which
	 * will again attempt to reclaim the already saved tm state.
	 * Hence we need to check that we've not already reclaimed
	 * this state.
	 * We do this using the current MSR, rather tracking it in
	 * some specific thread_struct bit, as it has the additional
	 * benefit of checking for a potential TM bad thing exception.
	 */
	if (!MSR_TM_SUSPENDED(mfmsr()))
		return;

	giveup_all(container_of(thr, struct task_struct, thread));

	tm_reclaim(thr, cause);

	/*
	 * If we are in a transaction and FP is off then we can't have
	 * used FP inside that transaction. Hence the checkpointed
	 * state is the same as the live state. We need to copy the
	 * live state to the checkpointed state so that when the
	 * transaction is restored, the checkpointed state is correct
	 * and the aborted transaction sees the correct state. We use
	 * ckpt_regs.msr here as that's what tm_reclaim will use to
	 * determine if it's going to write the checkpointed state or
	 * not. So either this will write the checkpointed registers,
	 * or reclaim will. Similarly for VMX.
	 */
	if ((thr->ckpt_regs.msr & MSR_FP) == 0)
		memcpy(&thr->ckfp_state, &thr->fp_state,
		       sizeof(struct thread_fp_state));
	if ((thr->ckpt_regs.msr & MSR_VEC) == 0)
		memcpy(&thr->ckvr_state, &thr->vr_state,
		       sizeof(struct thread_vr_state));
}