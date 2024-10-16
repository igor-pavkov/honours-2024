static void try_to_wake_up_local(struct task_struct *p)
{
	struct rq *rq = task_rq(p);

	if (WARN_ON_ONCE(rq != this_rq()) ||
	    WARN_ON_ONCE(p == current))
		return;

	lockdep_assert_held(&rq->lock);

	if (!raw_spin_trylock(&p->pi_lock)) {
		/*
		 * This is OK, because current is on_cpu, which avoids it being
		 * picked for load-balance and preemption/IRQs are still
		 * disabled avoiding further scheduler activity on it and we've
		 * not yet picked a replacement task.
		 */
		lockdep_unpin_lock(&rq->lock);
		raw_spin_unlock(&rq->lock);
		raw_spin_lock(&p->pi_lock);
		raw_spin_lock(&rq->lock);
		lockdep_pin_lock(&rq->lock);
	}

	if (!(p->state & TASK_NORMAL))
		goto out;

	trace_sched_waking(p);

	if (!task_on_rq_queued(p))
		ttwu_activate(rq, p, ENQUEUE_WAKEUP);

	ttwu_do_wakeup(rq, p, 0);
	if (schedstat_enabled())
		ttwu_stat(p, smp_processor_id(), 0);
out:
	raw_spin_unlock(&p->pi_lock);
}