void cgroup_move_task(struct task_struct *task, struct css_set *to)
{
	unsigned int task_flags = 0;
	struct rq_flags rf;
	struct rq *rq;

	if (static_branch_likely(&psi_disabled)) {
		/*
		 * Lame to do this here, but the scheduler cannot be locked
		 * from the outside, so we move cgroups from inside sched/.
		 */
		rcu_assign_pointer(task->cgroups, to);
		return;
	}

	rq = task_rq_lock(task, &rf);

	if (task_on_rq_queued(task))
		task_flags = TSK_RUNNING;
	else if (task->in_iowait)
		task_flags = TSK_IOWAIT;

	if (task->flags & PF_MEMSTALL)
		task_flags |= TSK_MEMSTALL;

	if (task_flags)
		psi_task_change(task, task_flags, 0);

	/* See comment above */
	rcu_assign_pointer(task->cgroups, to);

	if (task_flags)
		psi_task_change(task, 0, task_flags);

	task_rq_unlock(rq, task, &rf);
}