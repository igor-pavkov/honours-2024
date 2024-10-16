void psi_memstall_leave(unsigned long *flags)
{
	struct rq_flags rf;
	struct rq *rq;

	if (static_branch_likely(&psi_disabled))
		return;

	if (*flags)
		return;
	/*
	 * PF_MEMSTALL clearing & accounting needs to be atomic wrt
	 * changes to the task's scheduling state, otherwise we could
	 * race with CPU migration.
	 */
	rq = this_rq_lock_irq(&rf);

	current->flags &= ~PF_MEMSTALL;
	psi_task_change(current, TSK_MEMSTALL, 0);

	rq_unlock_irq(rq, &rf);
}