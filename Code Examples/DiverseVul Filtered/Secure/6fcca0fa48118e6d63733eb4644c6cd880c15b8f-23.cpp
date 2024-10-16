void psi_task_change(struct task_struct *task, int clear, int set)
{
	int cpu = task_cpu(task);
	struct psi_group *group;
	bool wake_clock = true;
	void *iter = NULL;

	if (!task->pid)
		return;

	if (((task->psi_flags & set) ||
	     (task->psi_flags & clear) != clear) &&
	    !psi_bug) {
		printk_deferred(KERN_ERR "psi: inconsistent task state! task=%d:%s cpu=%d psi_flags=%x clear=%x set=%x\n",
				task->pid, task->comm, cpu,
				task->psi_flags, clear, set);
		psi_bug = 1;
	}

	task->psi_flags &= ~clear;
	task->psi_flags |= set;

	/*
	 * Periodic aggregation shuts off if there is a period of no
	 * task changes, so we wake it back up if necessary. However,
	 * don't do this if the task change is the aggregation worker
	 * itself going to sleep, or we'll ping-pong forever.
	 */
	if (unlikely((clear & TSK_RUNNING) &&
		     (task->flags & PF_WQ_WORKER) &&
		     wq_worker_last_func(task) == psi_avgs_work))
		wake_clock = false;

	while ((group = iterate_groups(task, &iter))) {
		u32 state_mask = psi_group_change(group, cpu, clear, set);

		if (state_mask & group->poll_states)
			psi_schedule_poll_work(group, 1);

		if (wake_clock && !delayed_work_pending(&group->avgs_work))
			schedule_delayed_work(&group->avgs_work, PSI_FREQ);
	}
}