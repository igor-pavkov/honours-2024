void psi_memstall_enter(unsigned long *flags)
{
	struct rq_flags rf;
	struct rq *rq;

	if (static_branch_likely(&psi_disabled))
		return;

	*flags = current->flags & PF_MEMSTALL;
	if (*flags)
		return;
	/*
	 * PF_MEMSTALL setting & accounting needs to be atomic wrt
	 * changes to the task's scheduling state, otherwise we can
	 * race with CPU migration.
	 */
	rq = this_rq_lock_irq(&rf);

	current->flags |= PF_MEMSTALL;
	psi_task_change(current, 0, TSK_MEMSTALL);

	rq_unlock_irq(rq, &rf);
}