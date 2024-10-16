static void migrate_tasks(struct rq *dead_rq)
{
	struct rq *rq = dead_rq;
	struct task_struct *next, *stop = rq->stop;
	int dest_cpu;

	/*
	 * Fudge the rq selection such that the below task selection loop
	 * doesn't get stuck on the currently eligible stop task.
	 *
	 * We're currently inside stop_machine() and the rq is either stuck
	 * in the stop_machine_cpu_stop() loop, or we're executing this code,
	 * either way we should never end up calling schedule() until we're
	 * done here.
	 */
	rq->stop = NULL;

	/*
	 * put_prev_task() and pick_next_task() sched
	 * class method both need to have an up-to-date
	 * value of rq->clock[_task]
	 */
	update_rq_clock(rq);

	for (;;) {
		/*
		 * There's this thread running, bail when that's the only
		 * remaining thread.
		 */
		if (rq->nr_running == 1)
			break;

		/*
		 * pick_next_task assumes pinned rq->lock.
		 */
		lockdep_pin_lock(&rq->lock);
		next = pick_next_task(rq, &fake_task);
		BUG_ON(!next);
		next->sched_class->put_prev_task(rq, next);

		/*
		 * Rules for changing task_struct::cpus_allowed are holding
		 * both pi_lock and rq->lock, such that holding either
		 * stabilizes the mask.
		 *
		 * Drop rq->lock is not quite as disastrous as it usually is
		 * because !cpu_active at this point, which means load-balance
		 * will not interfere. Also, stop-machine.
		 */
		lockdep_unpin_lock(&rq->lock);
		raw_spin_unlock(&rq->lock);
		raw_spin_lock(&next->pi_lock);
		raw_spin_lock(&rq->lock);

		/*
		 * Since we're inside stop-machine, _nothing_ should have
		 * changed the task, WARN if weird stuff happened, because in
		 * that case the above rq->lock drop is a fail too.
		 */
		if (WARN_ON(task_rq(next) != rq || !task_on_rq_queued(next))) {
			raw_spin_unlock(&next->pi_lock);
			continue;
		}

		/* Find suitable destination for @next, with force if needed. */
		dest_cpu = select_fallback_rq(dead_rq->cpu, next);

		rq = __migrate_task(rq, next, dest_cpu);
		if (rq != dead_rq) {
			raw_spin_unlock(&rq->lock);
			rq = dead_rq;
			raw_spin_lock(&rq->lock);
		}
		raw_spin_unlock(&next->pi_lock);
	}

	rq->stop = stop;
}